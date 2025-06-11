#include "Tower.h"
#include "Camera/CameraComponent.h"

ATower::ATower()
{
    PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(false);
}



void ATower::BeginPlay()
{
    Super::BeginPlay();
}

void ATower::TurnTower(float ActionValue)
{
    TryTurn(ActionValue);
}

bool ATower::TryTurn(float ActionValue)
{
    if (!PlayerActor || !CameraPivot)
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing player or CameraPivot."));
        return false;
    }

    if (bIsTurning)
        return false;


    FVector Start = PlayerActor->GetActorLocation();
    FVector End = GetFutureCameraPosition(ActionValue);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(PlayerActor);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,
        Params
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

void ATower::CancelTurn()
{
    /*if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("TURN CANCELLED"));*/

    if (!CameraPivot)
        return;

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
        return;

    FVector OriginalLocation = Camera->GetRelativeLocation();
    float shaveValue = 1.f;
    FVector ShakeOffset = FVector(FMath::RandRange(-shaveValue, shaveValue), FMath::RandRange(-shaveValue, shaveValue), FMath::RandRange(-shaveValue, shaveValue));
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

FVector ATower::GetFutureCameraPosition(float ActionValue) const
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
    FRotator FutureRotation = CameraPivot->GetComponentRotation() + FRotator(0.f, 90.f * Direction, 0.f);
    FVector LocalOffset = Camera->GetRelativeLocation();

    return CameraPivot->GetComponentLocation() + FutureRotation.RotateVector(LocalOffset);
}

void ATower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsTurning)
        return;

    RotationTimer += DeltaTime;
    float RawAlpha = FMath::Clamp(RotationTimer / TurnDuration, 0.f, 1.f);
    float Alpha = FMath::InterpEaseInOut(0.f, 1.f, RawAlpha, 2.5f);

    FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Alpha);
    CameraPivot->SetWorldRotation(NewRotation);

    if (Alpha >= 1.0f)
    {
        CameraPivot->SetWorldRotation(TargetRotation);
        bIsTurning = false;
        SetActorTickEnabled(false);
    }
}

