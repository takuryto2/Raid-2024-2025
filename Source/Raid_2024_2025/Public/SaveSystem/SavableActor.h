// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Savable.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SavableActor.generated.h"

UCLASS()
class RAID_2024_2025_API ASavableActor : public AActor, public ISavable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASavableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool Recreate_Implementation() override;
};
