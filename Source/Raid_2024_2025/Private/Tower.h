#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/TowerInrerface.h"
#include "Tower.generated.h"

UCLASS()
class ATower : public AActor, public ITowerInrerface
{
    GENERATED_BODY()

    virtual void TurnTower(float ActionValue) override;
public:
    ATower();
    
    bool TryTurn(float ActionValue);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USceneComponent* CameraPivot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* PlayerActor;

protected:
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    void Turn(float ActionValue);
    void CancelTurn();
    FVector GetFutureCameraPosition(float ActionValue) const;


    UPROPERTY(EditAnywhere)
    float TurnDuration = 0.5f;

    UPROPERTY(BlueprintReadOnly)
    bool bIsTurning = false;

    float RotationTimer = 0.0f;
    int RotationDirection = 0;

    FRotator InitialRotation;
    FRotator TargetRotation;
};