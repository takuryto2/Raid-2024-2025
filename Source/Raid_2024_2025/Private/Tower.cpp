#include "Tower.h"

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
    if (bIsTurning /*|| FMath::IsNearlyZero(ActionValue)*/)
        return false;

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

