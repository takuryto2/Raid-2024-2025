#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CharacterPawnMovementComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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

    void PerformSlideAsyncTrace();
    void OnAsyncTraceResult(const FTraceHandle& Handle, FTraceDatum& Datum);
    bool CheckIfGrounded();

    // Courbes
    UPROPERTY(EditDefaultsOnly, Category = "Movement|Curves")
    TObjectPtr<UCurveFloat> SpeedCurve;

    UPROPERTY(EditDefaultsOnly, Category = "Movement|Curves")
    TObjectPtr<UCurveFloat> DashCurve;

    // Mouvement de base
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float VMax = 100.f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float AccelTime = 0.5f;

    // Masse / physique
    UPROPERTY(EditDefaultsOnly, Category = "Physics")
    float Mass = 70.f; // en kg

    UPROPERTY(EditDefaultsOnly, Category = "Physics")
    float Gravity = -980.f; // cm/s²

    // Saut
    UPROPERTY(EditDefaultsOnly, Category = "Jump")
    float JumpVelocity = 600.f;
    
    float VerticalSpeed = 0.f;
    bool bIsGrounded = true;

    // Dash
    UPROPERTY(EditDefaultsOnly, Category = "Dash")
    float DashDuration = 0.3f;

    UPROPERTY(EditDefaultsOnly, Category = "Dash")
    float DashCooldown = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Dash")
    float DashDistance = 1000.f;

    float DashTimer = 0.f;
    float DashCooldownTimer = 0.f;
    FVector2D DashDirection;
    bool bIsDashing = false;

    // Sol
    UPROPERTY(EditDefaultsOnly, Category = "Feet Trace")
    float FeetSkin = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "Feet Trace")
    float MaxGroundAngle = 45.f;

private:
    FVector2D CurrentDirection;
    float WalkProgress = 0.f;
    bool bCanMove = true;

    // Capsule
    float halfHeight = 0.0f;
    float CapsuleStep = 0.0f;
    float StepMult = 0.0f;
    FCollisionQueryParams CollisionParams;
    FCollisionShape FeetShape;
};
