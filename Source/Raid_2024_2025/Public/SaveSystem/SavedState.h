// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SavedState.generated.h"

#define LOG(what, ...) UE_LOG(LogTemp, Log, TEXT(what), ##__VA_ARGS__)

USTRUCT(Blueprintable, BlueprintType)
struct FSavedState
{
    GENERATED_BODY()

	public:
		TMap<FString, FString> strings;
		TMap<FString, int32>   ints;
		TMap<FString, float>   floats;
		TMap<FString, FVector> vectors;
	
	FString ToString() const
    {
        FString rv;

        rv += TEXT("\nStrings:\n");
        for (const TPair<FString, FString>& Pair : strings)
        {
            rv += FString::Printf(TEXT("  %s = \"%s\"\n"), *Pair.Key, *Pair.Value);
        }

        rv += TEXT("\nInts:\n");
        for (const TPair<FString, int32>& Pair : ints)
        {
            rv += FString::Printf(TEXT("  %s = %d\n"), *Pair.Key, Pair.Value);
        }

        rv += TEXT("\nFloats:\n");
        for (const TPair<FString, float>& Pair : floats)
        {
            rv += FString::Printf(TEXT("  %s = %.2f\n"), *Pair.Key, Pair.Value);
        }

        rv += TEXT("\nVectors:\n");
        for (const TPair<FString, FVector>& Pair : vectors)
        {
            rv += FString::Printf(TEXT("  %s = (X=%.2f, Y=%.2f, Z=%.2f)\n"), *Pair.Key, Pair.Value.X, Pair.Value.Y, Pair.Value.Z);
        }

        return rv;
    }

	void Log()
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *ToString())
	}

};