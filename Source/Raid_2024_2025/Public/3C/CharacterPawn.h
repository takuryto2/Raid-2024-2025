#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "CharacterPawn.generated.h"


class UCharacterPawnMovementComponent;

UCLASS()
class RAID_2024_2025_API ACharacterPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACharacterPawn();

protected:
#pragma region Input Config
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UInputConfig> InputConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UInputMappingContext> InputMappingContext;
#pragma endregion 
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyControllerChanged() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class APlayerController> _controller;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Move(const FInputActionValue& InputActionValue);
	virtual void Jump();
	virtual void Dash();
	virtual void MoveCancelled();
	void Turn(const FInputActionValue& InputActionValue);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCharacterPawnMovementComponent> MovementComponent;

	UPROPERTY()
	TArray<AActor*> Tower;

};
