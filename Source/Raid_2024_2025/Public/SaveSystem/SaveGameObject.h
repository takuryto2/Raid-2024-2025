// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Savable.h"
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
		UPROPERTY(VisibleAnywhere, Category = Basic)
		FString slotName;

		UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32  slotIndex;
	
	private:
		TMap<FString, FSavedState> objectToState;

	public:
		void Save();
		void Load();

		void GetAllSavableActors(TArray<TScriptInterface<ISavable>>& outSavable);
};
