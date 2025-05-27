// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SavedState.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FSavedState
{
    GENERATED_BODY()

	public:
		TMap<FString, FString> strings;
		TMap<FString, int32>   ints;
		TMap<FString, float>   floats;
		TMap<FString, FVector> vectors;
};