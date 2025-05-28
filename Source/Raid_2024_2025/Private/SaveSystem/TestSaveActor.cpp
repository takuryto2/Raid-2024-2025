// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem/TestSaveActor.h"

// Sets default values
ATestSaveActor::ATestSaveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATestSaveActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATestSaveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FSavedState* ATestSaveActor::GetState()
{
	FSavedState* rv = new FSavedState();
	rv->vectors.Add("pos", GetActorLocation());

	return rv;
}

void ATestSaveActor::SetState(FSavedState* state)
{
	SetActorLocation(state->vectors["pos"]);
}











void ATestSaveActor::TestSave()
{
	USaveGameObject* aled = Cast<USaveGameObject>(UGameplayStatics::CreateSaveGameObject(USaveGameObject::StaticClass()));

	if (!aled)
		return;

	aled->slotName = "kms";
	aled->slotIndex = 0;

	aled->Save(GetWorld());

	if (UGameplayStatics::SaveGameToSlot(aled, aled->slotName, aled->slotIndex))
		LOG("Saved game!")
}

void ATestSaveActor::TestLoad()
{
	USaveGameObject* aled = Cast<USaveGameObject>(UGameplayStatics::LoadGameFromSlot("kms", 0));

	if (!aled)
		return;

	aled->slotName = "kms";
	aled->slotIndex = 0;

	aled->Load(GetWorld());

	LOG("Loaded Game!")
}