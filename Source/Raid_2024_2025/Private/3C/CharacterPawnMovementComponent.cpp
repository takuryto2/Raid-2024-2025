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
    
    bCanDash = true;

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

    if (bIsGrounded)
    {
        bCanDash = true;
    }
    
    // Saut bufferé
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

    if (bIsGrounded)
        bCanDash = true;

    // --- DASH (framerate-indépendant) ---
    if (bIsDashing)
    {
        float PreviousAlpha = FMath::Clamp((DashTimer) / DashDuration, 0.f, 1.f);
        DashTimer += DeltaTime;
        float CurrentAlpha = FMath::Clamp(DashTimer / DashDuration, 0.f, 1.f);

        float PrevScale = DashCurve ? DashCurve->GetFloatValue(PreviousAlpha) : PreviousAlpha;
        float CurrScale = DashCurve ? DashCurve->GetFloatValue(CurrentAlpha) : CurrentAlpha;
        float DeltaScale = CurrScale - PrevScale;

        // Mouvement principal du dash (direction choisie au moment de l'input)
        FVector DashMove = DashDirection3D * DashDistance * DeltaScale;
        Movement += DashMove;

        // --- Ajout de mouvement latéral contrôlable pendant le dash ---
        if (!CurrentDirection.IsNearlyZero())
        {
            float LateralSpeed = VMax * 0.5f; // Réduction de la vitesse latérale pour ne pas dominer le dash
            FVector2D LateralVec = CurrentDirection * LateralSpeed * DeltaTime;
            FVector LateralMove = FVector(LateralVec.X, LateralVec.Y, 0.f);

            if (bIsGrounded)
            {
                FVector Projected = FVector::VectorPlaneProject(LateralMove, GroundNormal);
                Movement += Projected;
            }
            else
            {
                Movement += LateralMove;
            }
        }

        if (CurrentAlpha >= 1.f)
        {
            bIsDashing = false;
            bCanDash = false;
        }
    }


    // --- MARCHE ---
    else if (!CurrentDirection.IsNearlyZero() && bCanMove)
    {
        WalkProgress += DeltaTime / AccelTime;
        WalkProgress = FMath::Clamp(WalkProgress, 0.f, 1.f);

        float SpeedScale = SpeedCurve ? SpeedCurve->GetFloatValue(WalkProgress) : 1.f;
        CurrentSpeed = SpeedScale * VMax;

        FVector2D MoveVec = CurrentSpeed * DeltaTime * CurrentDirection;
        FVector Move = FVector(MoveVec.X, MoveVec.Y, 0);

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

    // --- GRAVITÉ ---
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
    if (!bIsDashing && bCanDash)
    {
        WalkProgress = 0.f;
        CurrentSpeed = 0.f;
        VerticalSpeed = 0.f;
        
        bIsDashing = true;
        DashTimer = 0.f;

        FVector DashInput(CurrentDirection.X, CurrentDirection.Y, 0.f);

        if (FMath::Abs(LastDashVerticalInput) > KINDA_SMALL_NUMBER)
        {
            DashInput.Z = LastDashVerticalInput;
        }

        DashDirection3D = DashInput.IsNearlyZero()
            ? FVector(CurrentRightDirection.X, CurrentRightDirection.Y, 0.f)
            : DashInput.GetSafeNormal();
    }

    CurrentFloorActor = nullptr;
}

void UCharacterPawnMovementComponent::UpdateRightDirection(const FVector2D& NewRightDirection)
{
    CurrentRightDirection = NewRightDirection;
}

void UCharacterPawnMovementComponent::MoveInput(const FVector2D& Input)
{
    if (Input.IsNearlyZero())
    {
        CurrentDirection = FVector2D::ZeroVector;
        LastDashVerticalInput = 0.f;
        return;
    }

    // Stocker uniquement l'input vertical (Y) pour le dash
    LastDashVerticalInput = Input.Y;

    // Utiliser uniquement l'axe X pour le déplacement (gauche/droite)
    FVector2D FinalInput(Input.X, 0.f);
    

    // Mouvement uniquement sur l'axe X (donc latéral), en tenant compte de l'orientation caméra
    FVector2D WorldDirection = FinalInput.X * CurrentRightDirection;

    LastDirection = FVector(WorldDirection.X, WorldDirection.Y, LastDashVerticalInput);

    if (FinalInput.IsNearlyZero())
    {
        CurrentDirection = FVector2D::ZeroVector;
        return;
    }

    if (FMath::Abs(Input.X) < MaxJoystickAngle)
    {
        FinalInput = FVector2D::ZeroVector;
        return;
    }

    CurrentDirection = WorldDirection.GetSafeNormal();

    GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan,
        FString::Printf(TEXT("Input: %s | MoveDir: %s | DashZ: %.2f"),
        *Input.ToString(), *CurrentDirection.ToString(), LastDashVerticalInput));
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
            CurrentFloorActor = GroundHit.GetActor();
            return true;
        }
    }

    CurrentFloorActor = nullptr;
    GroundNormal = FVector::UpVector;
    return false;
}
