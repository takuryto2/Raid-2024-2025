#include "3C/CharacterPawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"

UCharacterPawnMovementComponent::UCharacterPawnMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterPawnMovementComponent::BeginPlay()
{
    Super::BeginPlay();

    CollisionParams = FCollisionQueryParams(FName(TEXT("FeetTrace")), false, GetOwner());

    if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(UpdatedComponent))
    {
        FeetShape = FCollisionShape::MakeSphere(Capsule->GetScaledCapsuleRadius() - FeetSkin);
        halfHeight = Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere();
        CapsuleStep = Capsule->GetScaledCapsuleHalfHeight() - halfHeight;
        StepMult = 2.f;
    }
}

void UCharacterPawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DashCooldownTimer > 0.f)
        DashCooldownTimer -= DeltaTime;

    if (JumpBufferTimer > 0.f)
        JumpBufferTimer -= DeltaTime;

    FVector Movement = FVector::ZeroVector;

    bIsGrounded = CheckIfGrounded();

    // Saut depuis le buffer si on vient d'atterrir
    if (bIsGrounded && JumpBufferTimer > 0.f && VerticalSpeed <= 0.f)
    {
        VerticalSpeed = JumpVelocity;
        JumpBufferTimer = 0.f;
    }

    if (!bIsGrounded)
    {
        // Gravité
        const float GravityForce = (Mass * 2) * Gravity;
        const float GravityAcceleration = GravityForce / Mass;
        VerticalSpeed += GravityAcceleration * DeltaTime;

    }
    
    // Dash
    if (bIsDashing)
    {
        DashTimer += DeltaTime;
        float Alpha = FMath::Clamp(DashTimer / DashDuration, 0.f, 1.f);

        if (DashCurve)
        {
            float DashScale = DashCurve->GetFloatValue(Alpha);
            FVector2D MoveVec = DashDirection * DashDistance * DashScale;
            Movement += FVector(0, MoveVec.Y, MoveVec.X);
        }

        Movement.Z += VerticalSpeed * DeltaTime;

        if (Alpha >= 1.f)
        {
            bIsDashing = false;
            bCanMove = true;
            DashTimer = 0.f;
        }
    }
    // Déplacement normal
    else if (!CurrentDirection.IsNearlyZero() && bCanMove)
    {
        WalkProgress += DeltaTime / AccelTime;
        WalkProgress = FMath::Clamp(WalkProgress, 0.f, 1.f);

        float SpeedScale = SpeedCurve ? SpeedCurve->GetFloatValue(WalkProgress) : 1.f;
        CurrentSpeed = SpeedScale * VMax;
        FVector2D MoveVec = CurrentSpeed * DeltaTime * CurrentDirection;
        Movement += FVector(0, MoveVec.Y, 0);
    }

    // Appliquer gravité
    if (!bIsGrounded || VerticalSpeed > 0.f)
    {
        Movement.Z += VerticalSpeed * DeltaTime;
    }

    FHitResult Hit;
    SafeMoveUpdatedComponent(Movement, FQuat::Identity, true, Hit);

    if (Hit.IsValidBlockingHit())
    {
        SlideAlongSurface(Movement, 1.f - Hit.Time, Hit.Normal, Hit, true);
    }

    // Atterrissage
    if (bIsGrounded && VerticalSpeed < 0.f)
    {
        VerticalSpeed = 0.f;
    }

    if (!Movement.IsNearlyZero())
    {
        PerformSlideAsyncTrace();
    }
}

void UCharacterPawnMovementComponent::JumpInput()
{
    // Active un buffer pour un saut dès qu’on est au sol
    JumpBufferTimer = JumpBufferDuration;
}

void UCharacterPawnMovementComponent::DashInput()
{
    if (!bIsDashing && DashCooldownTimer <= 0.f)
    {
        bCanMove = false;
        bIsDashing = true;
        DashTimer = 0.f;
        DashDirection = CurrentDirection.IsZero() ? FVector2D(0, 1) : CurrentDirection.GetSafeNormal();
        DashCooldownTimer = DashCooldown + DashDuration;
    }
}

void UCharacterPawnMovementComponent::MoveInput(const FVector2D& Direction)
{
    CurrentDirection = Direction;
}

void UCharacterPawnMovementComponent::PerformSlideAsyncTrace()
{
    if (!GetWorld() || !UpdatedComponent)
        return;

    const FVector Start = UpdatedComponent->GetComponentLocation() + GetOwner()->GetActorUpVector() * -halfHeight;
    const FVector End = FVector(Start.X, Start.Y, Start.Z - CapsuleStep * StepMult);

    FTraceDelegate LocalDelegate;
    LocalDelegate.BindUObject(this, &UCharacterPawnMovementComponent::OnAsyncTraceResult);

    DrawDebugSphere(GetWorld(), Start, FeetShape.GetSphereRadius(), 16, FColor::Blue);
    DrawDebugSphere(GetWorld(), End, FeetShape.GetSphereRadius() / 10, 16, FColor::Red);

    GetWorld()->AsyncSweepByProfile(
        EAsyncTraceType::Single,
        Start, End, FQuat::Identity,
        UCollisionProfile::Pawn_ProfileName,
        FeetShape,
        CollisionParams,
        &LocalDelegate
    );
}

void UCharacterPawnMovementComponent::OnAsyncTraceResult(const FTraceHandle& Handle, FTraceDatum& Datum)
{
    if (!GetWorld()->QueryTraceData(Handle, Datum) || Datum.OutHits.IsEmpty())
        return;

    const FHitResult& Hit = Datum.OutHits[0];
#if WITH_EDITOR
    DrawDebugSphere(GetWorld(), Hit.Location, FeetShape.GetSphereRadius(), 16, FColor::Green);
#endif
}

bool UCharacterPawnMovementComponent::CheckIfGrounded()
{
    if (!GetWorld() || !UpdatedComponent)
        return false;

    FVector Start = UpdatedComponent->GetComponentLocation();
    FVector End = Start - FVector(0, 0, CapsuleStep * StepMult + 5);

    FHitResult GroundHit;
    bool bHit = GetWorld()->SweepSingleByChannel(
        GroundHit,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        FeetShape,
        CollisionParams
    );

#if WITH_EDITOR
    DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 1.f);
#endif

    if (bHit)
    {
        float HitAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GroundHit.Normal, FVector::UpVector)));
        return HitAngle <= MaxGroundAngle;
    }

    return false;
}