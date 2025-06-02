// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SavableState.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, MinimalAPI)
class USavableState : public UInterface
{
	GENERATED_BODY()
};

class RAID_2024_2025_API ISavableState
{
	GENERATED_BODY()

	public:
		// UFUNCTION(BlueprintCallable, Category = "Save/Load")
		virtual FSavedState GetState() = 0;

		// UFUNCTION(BlueprintCallable, Category = "Save/Load")
		virtual void SetState(const FSavedState& state) = 0;
};
