// Fill out your copyright notice in the Description page of Project Settings.


#include "3C/CharacterPawn.h"
#include "Config/Pawn/InputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "3C/CharacterPawnMovementComponent.h"

// Sets default values
ACharacterPawn::ACharacterPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MovementComponent = CreateDefaultSubobject<UCharacterPawnMovementComponent>(TEXT("Movement Component"));
}

// Called when the game starts or when spawned
void ACharacterPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACharacterPawn::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		_controller = PlayerController;
	}
}

// Called to bind functionality to input
void ACharacterPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	checkf(_controller, TEXT("No Pawn Controller found"));
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_controller -> GetLocalPlayer());
	Subsystem->RemoveMappingContext(InputMappingContext);
	Subsystem->AddMappingContext(InputMappingContext, 0);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent && InputConfig)
	{
		EnhancedInputComponent->BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ACharacterPawn::Move);
		EnhancedInputComponent->BindAction(InputConfig->MoveAction, ETriggerEvent::Completed, this, &ACharacterPawn::MoveCancelled);
		EnhancedInputComponent->BindAction(InputConfig->JumpAction, ETriggerEvent::Started, this, &ACharacterPawn::Jump);
		EnhancedInputComponent->BindAction(InputConfig->DashAction, ETriggerEvent::Started, this, &ACharacterPawn::Dash);
	}
}

void ACharacterPawn::Move(const FInputActionValue& InputActionValue)
{
	MovementComponent->MoveInput(InputActionValue.Get<FVector2D>());
}

void ACharacterPawn::MoveCancelled()
{
	MovementComponent->MoveInput(FVector2D(0, 0));
}

void ACharacterPawn::Jump()
{
	
}

void ACharacterPawn::Dash()
{
	
}

