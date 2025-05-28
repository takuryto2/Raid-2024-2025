// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Savable.h"
#include "SavedState.h"
#include "SaveGameObject.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestSaveActor.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RAID_2024_2025_API ATestSaveActor : public AActor, public ISavable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestSaveActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// UFUNCTION(BlueprintCallable, Category = "Save/Load")
	virtual FSavedState* GetState() override;
		
	// UFUNCTION(BlueprintCallable, Category = "Save/Load")
	virtual void SetState(FSavedState* state) override;

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void TestSave();

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void TestLoad();
};
