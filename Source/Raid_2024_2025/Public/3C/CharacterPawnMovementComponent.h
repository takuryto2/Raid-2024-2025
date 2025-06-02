#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CharacterPawnMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RAID_2024_2025_API UCharacterPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UCharacterPawnMovementComponent();

	void JumpInput();
	void DashInput();
	void MoveInput(const FVector2D& Direction);

	float CurrentSpeed = 0;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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

#pragma region Slope Slide
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slope")
	float MaxGroundAngle = 45.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slope")
	float SlopeSlideSpeed = 600.f;
#pragma endregion

private:
	FVector2D CurrentDirection;
	float WalkProgress = 0;
	bool bCanMove = true;
	bool CheckIfGrounded();

#pragma region Dash
	UPROPERTY(EditDefaultsOnly, Category="Movement|Dash")
	float DashTime = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category="Movement|Dash")
	float DashSpeed = 2500.0f;

	UPROPERTY(EditDefaultsOnly, Category="Movement|Dash")
	float DashCooldown = 1.0f;

	bool bIsDashing = false;
	float DashTimer = 0.0f;
	FVector2D DashDirection;
	float DashCooldownTimer = 0.0f;
#pragma endregion

#pragma region Feet Trace
	float halfHeight = 0.0f;
	float CapsuleStep = 0.0f;
	float StepMult = 0.0f;
	FCollisionQueryParams CollisionParams;
	FTraceDelegate TraceDelegate;
	FCollisionShape FeetShape;
#pragma endregion

#pragma region Jump
	bool bIsJumping = false;
	float JumpVelocity = 900.f;
	float VerticalSpeed = 0.f;
	float Gravity = -2000.f;
	bool bIsGrounded = true;
#pragma endregion
};