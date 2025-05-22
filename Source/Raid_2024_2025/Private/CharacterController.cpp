#include "CharacterController.h"

#include <iostream>

#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"

ACharacterController::ACharacterController()
{
    PrimaryActorTick.bCanEverTick = true;

    // Constrain le mouvement uniquement au plan Y
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->SetPlaneConstraintNormal(FVector(1, 0, 0));
}

void ACharacterController::BeginPlay()
{
    Super::BeginPlay();
}

void ACharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACharacterController::Move(float ActionValue)
{
    AddMovementInput(FVector(0.f, ActionValue, 0.f), MoveSpeed);
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, *FString::SanitizeFloat(ActionValue));
}

void ACharacterController::Dash(FVector2D ActionVector2)
{
    if (ActionVector2.Y > 0.f)
    {
        FVector DashDirection = FVector(ActionVector2, 0.f) * DashForce;
        LaunchCharacter(DashDirection, true, true);
    }
    else
    {
        LaunchCharacter(GetActorForwardVector() * DashForce, false, false);
    }
}

