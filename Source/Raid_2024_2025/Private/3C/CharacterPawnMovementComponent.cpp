#include "3C/CharacterPawnMovementComponent.h" 
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"

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

    GroundNormal = FVector::UpVector;
}

void UCharacterPawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DashCooldownTimer > 0.f)
        DashCooldownTimer -= DeltaTime;

    if (JumpBufferTimer > 0.f)
        JumpBufferTimer -= DeltaTime;

    // ---- PLATFORM MOVEMENT SYNC ----
    if (CurrentFloorActor)
    {
        FTransform CurrentTransform = CurrentFloorActor->GetActorTransform();

        if (!bWasOnPlatformLastFrame)
        {
            FString PlatformName = CurrentFloorActor->GetName();
            UE_LOG(LogTemp, Log, TEXT("Connected to platform: %s"), *PlatformName);
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, FString::Printf(TEXT("Connected to: %s"), *PlatformName));
        }

        if (bWasOnPlatformLastFrame)
        {
            FTransform DeltaTransform = CurrentTransform.GetRelativeTransform(PreviousPlatformTransform);
            FVector PlatformDelta = DeltaTransform.GetTranslation();

            if (!PlatformDelta.IsNearlyZero())
            {
                UpdatedComponent->AddWorldOffset(PlatformDelta, true);
            }

            // Optional rotation
             FQuat RotationDelta = DeltaTransform.GetRotation();
             UpdatedComponent->AddWorldRotation(RotationDelta.Rotator(), true);
        }

        PreviousPlatformTransform = CurrentTransform;
        bWasOnPlatformLastFrame = true;
    }
    else
    {
        bWasOnPlatformLastFrame = false;
    }



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
            FVector MoveVec = DashDirection3D * DashDistance * DashScale;
            Movement += MoveVec;
        }

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
        FVector Move = FVector(MoveVec.X, MoveVec.Y, 0);

        // Coller au sol uniquement si on bouge
        if (bIsGrounded)
        {
            FVector Projected = FVector::VectorPlaneProject(Move, GroundNormal);
            Movement += Projected;
        }
        else
        {
            Movement += Move;
        }
    }

    // Gravité (appliquée seulement si en l'air ou en montée)
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
    JumpBufferTimer = JumpBufferDuration;
    CurrentFloorActor = nullptr;
}

void UCharacterPawnMovementComponent::DashInput()
{
    if (!bIsDashing && DashCooldownTimer <= 0.f)
    {
        WalkProgress = 0.f;
        CurrentSpeed = 0.f;
        VerticalSpeed = 0.f;
        
        bCanMove = false;
        bIsDashing = true;
        DashTimer = 0.f;
        DashCooldownTimer = DashCooldown + DashDuration;

        FVector DashInput = FVector(CurrentDirection.X, CurrentDirection.Y, 0.f);

        // Gestion verticale (Z = haut, S = bas)
        APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
        if (PC && PC->IsInputKeyDown(EKeys::Z))
        {
            DashInput.Z += 1.f;
        }
        if (PC && PC->IsInputKeyDown(EKeys::S))
        {
            DashInput.Z -= 1.f;
        }

        DashDirection3D = DashInput.IsNearlyZero() ? FVector(CurrentRightDirection.X, CurrentRightDirection.Y, 0) : DashInput.GetSafeNormal();
    }
    CurrentFloorActor = nullptr;
}

void UCharacterPawnMovementComponent::UpdateRightDirection(const FVector2D& NewRightDirection)
{
    CurrentRightDirection = NewRightDirection;
}



void UCharacterPawnMovementComponent::MoveInput(const FVector2D& Direction)
{
    FVector2D LocalCurrentDirection = CurrentRightDirection * FVector2D(Direction.Y); //Multiplier par la droite de la camera
    LocalCurrentDirection.Normalize();
    CurrentDirection = LocalCurrentDirection;
    GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::Printf(TEXT("Current Direction: %s"), *CurrentDirection.ToString()));
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
    FVector End = Start - FVector(0, 0, CapsuleStep * StepMult + 7.5f);

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
        GroundNormal = GroundHit.Normal;

        float HitAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GroundHit.Normal, FVector::UpVector)));
        if (HitAngle <= MaxGroundAngle)
        {
            // 👇 Set the actor you're standing on
            CurrentFloorActor = GroundHit.GetActor();
            return true;
        }
    }

    // Reset if not grounded
    CurrentFloorActor = nullptr;
    GroundNormal = FVector::UpVector;
    return false;
}