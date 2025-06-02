// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Savable.h"
#include "SavableState.h"
#include "SavedState.h"
#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SaveGame.h"

#include "SaveGameObject.generated.h"

UCLASS(Blueprintable, BlueprintType)
class RAID_2024_2025_API USaveGameObject : public USaveGame
{
	GENERATED_BODY()

	public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
		FString slotName;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
		int32 slotIndex;
	
	private:
		UPROPERTY()
		TMap<FString, FSavedState> objectToState;

	public:
		USaveGameObject();

		UFUNCTION(BlueprintCallable, Category = "Save System")
		void Save(UObject* worldContextObject);

		UFUNCTION(BlueprintCallable, Category = "Save System")
		void Load(UObject* worldContextObject);

		void GetAllSavableActors(const UWorld* world, TArray<AActor*>& outSavable);
};
