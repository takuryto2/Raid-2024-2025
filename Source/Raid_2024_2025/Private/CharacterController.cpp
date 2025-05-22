#include "CharacterController.h"
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
    AddMovementInput(FVector(ActionValue, ActionValue, 0.f), MoveSpeed);
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("Move: ") + FString::SanitizeFloat(ActionValue));
}

void ACharacterController::Dash(FVector2D ActionVector2)
{
    if (!CanDash)
        return;

    CanDash = false;
    float tempDashForce = DashForce;

    if (ActionVector2 < FVector2D::ZeroVector)
    {
        tempDashForce = -DashForce;
    }
    else
    {
        tempDashForce = DashForce;
    }

    FVector DashDirection = FVector(ActionVector2, 0.f) * tempDashForce;

    // Vérifie si le personnage est en l'air
    if (!GetCharacterMovement()->IsMovingOnGround())
    {
        // Applique une friction supplémentaire
        GetCharacterMovement()->BrakingFrictionFactor = DashFriction;
        DashAir = true;
    }
    else
    {
        GetCharacterMovement()->GroundFriction = 0.0f;
    }

    LaunchCharacter(DashDirection, true, true);

    // Attendre 1 seconde puis réinitialiser la friction
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
    {
        GetCharacterMovement()->GroundFriction = 1.0f;
        if (!DashAir)
        {
            CanDash = true;
        }
    }, 1.0f, false);
}

void ACharacterController::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    if (!CanDash && DashAir)
    {
        GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
        DashAir = false;
        CanDash = true;
    }
}
