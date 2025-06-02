// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Map.h"
#include "SavedState.h"
#include "Savable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, MinimalAPI)
class USavable : public UInterface
{
	GENERATED_BODY()
};

class RAID_2024_2025_API ISavable
{
	GENERATED_BODY()

	public:
		// UFUNCTION(BlueprintCallable, Category = "Save/Load")
		virtual FSavedState GetState() = 0;

		// UFUNCTION(BlueprintCallable, Category = "Save/Load")
		virtual void SetState(const FSavedState& state) = 0;
};
