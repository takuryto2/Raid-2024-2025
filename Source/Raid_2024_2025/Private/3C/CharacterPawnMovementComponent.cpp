#include "3C/CharacterPawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "GameFramework/Actor.h"

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
        StepMult = 5.0f;
    }
}

void UCharacterPawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DashCooldownTimer > 0.0f)
        DashCooldownTimer -= DeltaTime;

    FVector Movement = FVector::ZeroVector;

    if (bIsDashing)
    {
        DashTimer -= DeltaTime;
        FVector2D MoveVector2 = DashSpeed * DeltaTime * DashDirection;
        Movement = FVector(0, MoveVector2.Y, MoveVector2.X);

        if (DashTimer <= 0.0f)
            bIsDashing = false;
    }
    else if (!CurrentDirection.IsNearlyZero())
    {
        if (bCanMove)
        {
            WalkProgress += DeltaTime / AccelTime;
            WalkProgress = FMath::Clamp(WalkProgress, 0.f, 1.f);

            CurrentSpeed = SpeedCurve->GetFloatValue(WalkProgress) * VMax;
            FVector2D MoveVector2 = CurrentSpeed * DeltaTime * CurrentDirection;
            Movement = FVector(0, MoveVector2.Y, 0);
        }
    }

    if (bIsJumping || !bIsGrounded)
    {
        VerticalSpeed += Gravity * DeltaTime;
        Movement.Z = VerticalSpeed * DeltaTime;
    }

    FHitResult Hit;
    SafeMoveUpdatedComponent(Movement, FQuat::Identity, true, Hit);

    if (Hit.IsValidBlockingHit())
    {
        SlideAlongSurface(Movement, 1.f - Hit.Time, Hit.Normal, Hit, true);

        const FVector Normal = Hit.Normal;
        const float HitAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Normal, FVector::UpVector)));

        if (HitAngle <= MaxGroundAngle)
        {
            bIsJumping = false;
            bIsGrounded = true;
            VerticalSpeed = 0.f;
        }
        else
        {
            bIsGrounded = false;

            const FVector SlideDir = FVector::VectorPlaneProject(FVector::DownVector, Hit.Normal).GetSafeNormal();
            Movement += SlideDir * SlopeSlideSpeed * DeltaTime;

            FHitResult SlideHit;
            SafeMoveUpdatedComponent(SlideDir * SlopeSlideSpeed * DeltaTime, FQuat::Identity, true, SlideHit);
        }
    }
    else
    {
        bIsGrounded = false;
    }

    if (!Movement.IsNearlyZero())
    {
        PerformSlideAsyncTrace(Movement);
    }
}

void UCharacterPawnMovementComponent::JumpInput()
{
    if (bIsGrounded)
    {
        bIsJumping = true;
        bIsGrounded = false;
        VerticalSpeed = JumpVelocity;
    }
}

void UCharacterPawnMovementComponent::DashInput()
{
    bCanMove = false;
    if (!bIsDashing && DashCooldownTimer <= 0.0f)
    {
        bIsDashing = true;
        DashTimer = DashTime;
        DashDirection = CurrentDirection.IsZero() ? FVector2D(0, 1) : CurrentDirection;
        DashCooldownTimer = DashCooldown + DashTime;
    }
    MoveInput(FVector2D(0, 0));
    bCanMove = true;
}

void UCharacterPawnMovementComponent::MoveInput(const FVector2D& Direction)
{
    FVector2D AdjustedDirection = Direction;

    if (bIsGrounded && Direction.Y < -0.5f && FMath::Abs(Direction.X) > 0.1f)
    {
        AdjustedDirection.Y = 0;
    }

    CurrentDirection = AdjustedDirection.GetSafeNormal();
}

void UCharacterPawnMovementComponent::PerformSlideAsyncTrace(const FVector& MoveVector)
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
    if (!GetWorld()->GetWorld()->QueryTraceData(Handle, Datum) || Datum.OutHits.IsEmpty())
        return;

    const FHitResult& Hit = Datum.OutHits[0];
    #if WITH_EDITOR
    DrawDebugSphere(GetWorld(), Hit.Location, FeetShape.GetSphereRadius(), 16, FColor::Green);
    #endif
}
