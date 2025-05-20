#include "CharacterController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACharacterController::ACharacterController()
{
    PrimaryActorTick.bCanEverTick = true;

    // Lock le mouvement à Y uniquement (axe horizontal du plan X/Y)
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;

    // Caméra
    USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 500.f;
    SpringArm->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    SpringArm->bUsePawnControlRotation = false;

    UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

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
    PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterController::MoveRight);
}

void ACharacterController::MoveRight(float Value)
{
    AddMovementInput(FVector(0.f, 1.f, 0.f), Value);
}
