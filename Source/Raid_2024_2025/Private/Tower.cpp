#include "Tower.h"

#include "3C/CharacterPawnMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/StaticMeshComponent.h"

ATower::ATower()
{
    PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(false);
}

void ATower::BeginPlay()
{
    Super::BeginPlay();
}

bool ATower::TryTurn(float ActionValue)
{
    if (!PlayerActor || !CameraPivot)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayerActor or CameraPivot is null."));
        return false;
    }

    if (bIsTurning)
        return false;

    float Direction = FMath::Sign(ActionValue);

    FVector PlayerLocation = PlayerActor->GetActorLocation();
    FVector NextPlayerLocation = PlayerLocation;

    if (Direction > 0.f && LeftAnchor)
    {
        NextPlayerLocation.X = LeftAnchor->GetComponentLocation().X;
        NextPlayerLocation.Y = LeftAnchor->GetComponentLocation().Y;
    }
    else if (Direction < 0.f && RightAnchor)
    {
        NextPlayerLocation.X = RightAnchor->GetComponentLocation().X;
        NextPlayerLocation.Y = RightAnchor->GetComponentLocation().Y;
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Missing anchor for this direction."));
        return false;
    }

    // Box trace
    FVector BoxHalfSize = FVector(10.f, 10.f, 10.f);
    FRotator BoxRotation = FRotator::ZeroRotator;
    FHitResult HitResult;
    bool bHit = UKismetSystemLibrary::BoxTraceSingle(
        GetWorld(),
        PlayerLocation,
        NextPlayerLocation,
        BoxHalfSize,
        BoxRotation,
        UEngineTypes::ConvertToTraceType(ECC_Visibility),
        false,
        { this, PlayerActor },
        EDrawDebugTrace::ForDuration,
        HitResult,
        true,
        FLinearColor::Red,
        FLinearColor::Green,
        2.0f
    );

    if (bHit)
    {
        CancelTurn();
        return false;
    }

    bIsTurning = true;
    Turn(ActionValue);
    return true;
}



void ATower::Turn(float ActionValue)
{
    RotationDirection = FMath::Sign(ActionValue); // +1 or -1
    RotationTimer = 0.0f;

    InitialRotation = CameraPivot->GetComponentRotation();
    TargetRotation = InitialRotation + FRotator(0.f, 90.f * RotationDirection, 0.f);

    SetActorTickEnabled(true);
}

void ATower::TurnInput(float ActionValue, ACharacterPawn* CharacterPawn)
{
    if (!CharacterPawn)
        return;
    

    // Désactive le mouvement du pawn
    if (UCharacterPawnMovementComponent* MoveComp = Cast<UCharacterPawnMovementComponent>(CharacterPawn->FindComponentByClass<UCharacterPawnMovementComponent>()))
    {
        MoveComp->SetCanMove(false);
    }

    TryTurn(ActionValue);

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Turning Tower..." + FString::SanitizeFloat(ActionValue)));
}

void ATower::CancelTurn()
{
    if (!CameraPivot)
        return;

    UCameraComponent* Camera = nullptr;
    for (USceneComponent* Child : playerCameraPivot->GetAttachChildren())
    {
        if (UCameraComponent* FoundCamera = Cast<UCameraComponent>(Child))
        {
            Camera = FoundCamera;
            break;
        }
    }

    if (!Camera)
        return;

    FVector OriginalLocation = Camera->GetRelativeLocation();
    float shaveValue = 1.f;
    FVector ShakeOffset = FVector(
        FMath::RandRange(-shaveValue, shaveValue),
        FMath::RandRange(-shaveValue, shaveValue),
        FMath::RandRange(-shaveValue, shaveValue)
    );
    Camera->SetRelativeLocation(OriginalLocation + ShakeOffset);

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [Camera, OriginalLocation]()
    {
        if (Camera)
        {
            Camera->SetRelativeLocation(OriginalLocation);
        }
    }, 0.05f, false);
}

FVector ATower::GetNextCameraPosition(float ActionValue) const
{
    if (!CameraPivot)
    {
        UE_LOG(LogTemp, Warning, TEXT("CameraPivot is null"));
        return FVector::ZeroVector;
    }

    UCameraComponent* Camera = nullptr;
    for (USceneComponent* Child : CameraPivot->GetAttachChildren())
    {
        if (UCameraComponent* FoundCamera = Cast<UCameraComponent>(Child))
        {
            Camera = FoundCamera;
            break;
        }
    }

    if (!Camera)
    {
        UE_LOG(LogTemp, Warning, TEXT("No camera found among CameraPivot's children"));
        return FVector::ZeroVector;
    }

    float Direction = FMath::Sign(ActionValue);
    FRotator NextRotation = CameraPivot->GetComponentRotation() + FRotator(0.f, 90.f * Direction, 0.f);
    FVector LocalOffset = Camera->GetRelativeLocation();

    return CameraPivot->GetComponentLocation() + NextRotation.RotateVector(LocalOffset);
}

void ATower::UpdateCharacterRightDirection()
{
    if (!CameraPivot || !PlayerActor)
        return;

    ACharacterPawn* CharacterPawn = Cast<ACharacterPawn>(PlayerActor);
    if (!CharacterPawn)
        return;

    UCameraComponent* Camera = nullptr;
    for (USceneComponent* Child : playerCameraPivot->GetAttachChildren())
    {
        if (UCameraComponent* FoundCamera = Cast<UCameraComponent>(Child))
        {
            Camera = FoundCamera;
            break;
        }
    }

    if (!Camera)
        return;

    FVector RightVector = Camera->GetRightVector();
    FVector2D RightDirection(RightVector.X, RightVector.Y);

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Setting new RightDirection: " + RightDirection.ToString()));

    CharacterPawn->SetRightDirection(RightDirection);
}

void ATower::LerpPlayer()
{
    if (!CameraPivot || !PlayerActor)
        return;

    FVector BackwardDirection = -playerCameraPivot->GetForwardVector();
    FVector Offset = BackwardDirection * OffsetDistance;

    PlayerLerpStart = PlayerActor->GetActorLocation();

    PlayerLerpTarget = PlayerLerpStart + Offset;

    if (RotationDirection < 0 && LeftAnchor)
    {
        PlayerLerpTarget.X = LeftAnchor->GetComponentLocation().X;
        PlayerLerpTarget.Y = LeftAnchor->GetComponentLocation().Y;
    }
    else if (RotationDirection > 0 && RightAnchor)
    {
        PlayerLerpTarget.X = RightAnchor->GetComponentLocation().X;
        PlayerLerpTarget.Y = RightAnchor->GetComponentLocation().Y;
    }


    PlayerLerpTimer = 0.f;
    bIsLerpingPlayer = true;
}


void ATower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsTurning)
    {
        RotationTimer += DeltaTime;
        float RawAlpha = FMath::Clamp(RotationTimer / TurnDuration, 0.f, 1.f);
        float Alpha = FMath::InterpEaseInOut(0.f, 1.f, RawAlpha, 2.5f);

        FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Alpha);
        CameraPivot->SetWorldRotation(NewRotation);

        if (Alpha >= 1.0f)
        {
            bIsTurning = false;

            // Réactive le mouvement du pawn
            if (PlayerActor)
            {
                if (ACharacterPawn* CharacterPawn = Cast<ACharacterPawn>(PlayerActor))
                {
                    if (UCharacterPawnMovementComponent* MoveComp = Cast<UCharacterPawnMovementComponent>(CharacterPawn->FindComponentByClass<UCharacterPawnMovementComponent>()))
                    {
                        MoveComp->SetCanMove(true);
                    }
                }
            }

            UpdateCharacterRightDirection();
            LerpPlayer();
        }
    }

    if (bIsLerpingPlayer && PlayerActor)
    {
        PlayerLerpTimer += DeltaTime;
        float Alpha = FMath::Clamp(PlayerLerpTimer / PlayerLerpDuration, 0.f, 1.f);
        FVector NewLocation = FMath::Lerp(PlayerLerpStart, PlayerLerpTarget, Alpha);
        PlayerActor->SetActorLocation(NewLocation);

        if (Alpha >= 1.0f)
        {
            bIsLerpingPlayer = false;
        }
    }
}


