#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterController.generated.h"

UCLASS()
class RAID_2024_2025_API ACharacterController : public ACharacter
{
    GENERATED_BODY()

public:
    ACharacterController();
    
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveSpeed = 600.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float DashForce = 6.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float DashFriction = 4.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    bool CanDash = true;

protected:
    virtual void BeginPlay() override;
    virtual void Landed(const FHitResult& Hit) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, Category = "Movement")
    bool DashAir = false;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void Move(float ActionValue);

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void Dash(FVector2D ActionVector2);
};
