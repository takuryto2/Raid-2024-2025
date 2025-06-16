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
        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<FString, FString> strings;

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<FString, int32>   ints;

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<FString, double>   doubles;

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<FString, FVector> vectors;

        // set by the save object
        UPROPERTY()
        UClass* objectType;

        UPROPERTY()
        FTransform objectTransform;
    
        FString ToString() const
        {
            FString name = strings.Contains(NAME) ? strings[NAME] : TEXT("unknown");
            FString rv = FString::Format(TEXT("State of object '{0}' ({1})"), {*name, *objectType->GetName()});
    
            rv += TEXT("\n    Strings:\n");
            for (const TPair<FString, FString>& Pair : strings)
            {
                rv += FString::Printf(TEXT("        %s = \"%s\"\n"), *Pair.Key, *Pair.Value);
            }
    
            rv += TEXT("\n    Ints:\n");
            for (const TPair<FString, int32>& Pair : ints)
            {
                rv += FString::Printf(TEXT("        %s = %d\n"), *Pair.Key, Pair.Value);
            }
    
            rv += TEXT("\n    Doubles:\n");
            for (const TPair<FString, float>& Pair : doubles)
            {
                rv += FString::Printf(TEXT("        %s = %.2f\n"), *Pair.Key, Pair.Value);
            }
    
            rv += TEXT("\n    Vectors:\n");
            for (const TPair<FString, FVector>& Pair : vectors)
            {
                rv += FString::Printf(TEXT("        %s = (X=%.2f, Y=%.2f, Z=%.2f)\n"), *Pair.Key, Pair.Value.X, Pair.Value.Y, Pair.Value.Z);
            }
    
            return rv;
        }
    
        void Log()
        {
            UE_LOG(LogTemp, Log, TEXT("%s"), *ToString())
        }

    // special key for `ints`, set by the save object
    // if the value is 1 and that the object is not in the world when loading a scene, will respawn this object
    static const FString SHOULD_RECREATE;

    static const FString NAME;
    static const FString FULLNAME;
	
};