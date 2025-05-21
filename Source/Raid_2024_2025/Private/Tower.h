#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tower.generated.h"

UCLASS()
class ATower : public AActor
{
    GENERATED_BODY()

public:
    ATower();

    UFUNCTION(BlueprintCallable)
    bool TryTurn(float ActionValue); 

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USceneComponent* CameraPivot;


protected:
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    void Turn(float ActionValue);

    UPROPERTY(EditAnywhere)
    float TurnDuration = 0.5f;

    UPROPERTY(BlueprintReadOnly)
    bool bIsTurning = false;

    float RotationTimer = 0.0f;
    int RotationDirection = 0;

    FRotator InitialRotation;
    FRotator TargetRotation;
};