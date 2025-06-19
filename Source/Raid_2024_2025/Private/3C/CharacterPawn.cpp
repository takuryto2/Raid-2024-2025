// Fill out your copyright notice in the Description page of Project Settings.


#include "3C/CharacterPawn.h"

#include "EngineUtils.h"
#include "Config/Pawn/InputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Tower.h"
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

	// Recherche du premier ATower dans la scène
	for (TActorIterator<ATower> It(GetWorld()); It; ++It)
	{
		TowerActor = *It;
		break; // Prend le premier trouvé
	}

	if (!TowerActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Aucun TowerActor trouvé dans la scène."));
	}
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
		EnhancedInputComponent->BindAction(InputConfig->TurnAction, ETriggerEvent::Triggered, this, &ACharacterPawn::Turn);
	}
}

void ACharacterPawn::Move(const FInputActionValue& InputActionValue)
{
	MovementComponent->MoveInput(InputActionValue.Get<FVector2D>());
}

void ACharacterPawn::SetRightDirection(FVector2D TowerRightDirection)
{
	MovementComponent->UpdateRightDirection(TowerRightDirection);
}


void ACharacterPawn::MoveCancelled()
{
	MovementComponent->MoveInput(FVector2D(0, 0));
}

void ACharacterPawn::Jump()
{
	MovementComponent->JumpInput();
}

void ACharacterPawn::Dash()
{
	MovementComponent->DashInput();
}

void ACharacterPawn::Turn(const FInputActionValue& InputActionValue)
{
	if (TowerActor)
	{
		if (ATower* Tower = Cast<ATower>(TowerActor))
		{
			Tower->TurnInput(InputActionValue.Get<float>(), this);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("TowerActor is not of type ATower."));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("TowerActor is null."));
	}
}

