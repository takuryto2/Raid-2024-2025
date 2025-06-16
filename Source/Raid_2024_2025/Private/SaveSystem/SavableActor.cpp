// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem/SavableActor.h"

ALWAYS_RECREATE(ASavableActor)
DEFAULT_GETSTATE(ASavableActor)
DEFAULT_SETSTATE(ASavableActor)

// Sets default values
ASavableActor::ASavableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASavableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASavableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

