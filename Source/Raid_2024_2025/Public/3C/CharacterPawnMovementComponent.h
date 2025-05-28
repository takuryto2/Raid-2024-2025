// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CharacterPawnMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RAID_2024_2025_API UCharacterPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterPawnMovementComponent();

	void JumpInput();
	void DashInput();
	void MoveInput(const FVector2D& Direction);
	
	float CurrentSpeed = 0;

protected:
	float halfHeight = 0.0f;
	float CapsuleStep = 0.0f;
	float StepMult = 0.0f;

	// Called when the game starts
	virtual void BeginPlay() override;

	void PerformSlideAsyncTrace(const FVector& DesiredMove);

	void OnAsyncTraceResult(const FTraceHandle& Handle, FTraceDatum& Datum);
	
	UPROPERTY(EditDefaultsOnly, Category="Acceleration")
	TObjectPtr<UCurveFloat> SpeedCurve;

	UPROPERTY(EditDefaultsOnly, Category="Acceleration")
	float VMax = 100;

	UPROPERTY(EditDefaultsOnly, Category="Acceleration")
	float AccelTime = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float FeetSkin = 1;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	FVector2D CurrentDirection;

	float WalkProgress = 0;

	bool bCanMove = true;

#pragma region Dash
	UPROPERTY(EditDefaultsOnly, Category="Movement|Dash")
	float DashTime = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category="Movement|Dash")
	float DashSpeed = 2500.0f;

	UPROPERTY(EditDefaultsOnly, Category="Movement|Dash")
	float DashCooldown = 1.0f;
#pragma endregion 

#pragma region Feet Trace
	float FeetBaseHeight = 0;
	FCollisionQueryParams CollisionParams;
	FTraceDelegate TraceDelegate;
	FCollisionShape FeetShape;
#pragma endregion

#pragma region Dash
	bool bIsDashing = false;
	float DashTimer = 0.0f;
	FVector2D DashDirection;
	float DashCooldownTimer = 0.0f;
#pragma endregion 
};
