// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TowerInrerface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTowerInrerface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RAID_2024_2025_API ITowerInrerface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void TurnTower(float ActionValue) = 0;
	
};
