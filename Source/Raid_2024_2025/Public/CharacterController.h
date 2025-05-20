#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterController.generated.h"

UCLASS()
class SIDESCROLLER3D_API ACharacterController : public ACharacter
{
    GENERATED_BODY()

public:
    ACharacterController();

protected:
    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void MoveRight(float Value);

};