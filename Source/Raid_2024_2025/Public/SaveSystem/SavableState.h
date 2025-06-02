// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SavedState.h"
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
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
		FSavedState GetState();

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
		void SetState(const FSavedState& state);
};

#define DEFAULT_GETSTATE(cls) \
FSavedState cls::GetState_Implementation() \
{ \
    FSavedState state; \
    return state; \
} 

#define DEFAULT_SETSTATE(cls) \
void cls::SetState_Implementation() { } 