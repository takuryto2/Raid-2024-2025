// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/SaveGameObject.h"

void USaveGameObject::GetAllSavableActors(const UWorld* world, TArray<AActor*>& outSavable)
{
    UGameplayStatics::GetAllActorsWithInterface(world, USavable::StaticClass(), outSavable);
}

void USaveGameObject::Save(const UWorld* world)
{
    TArray<AActor*> savableActors;
    GetAllSavableActors(world, savableActors);

    for (AActor* savableActor : savableActors)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, savableActor->GetName());

        FString actorName = savableActor->GetFullName();
        FSavedState* state = Cast<ISavable>(savableActor)->GetState();
        objectToState.Add(actorName, *state);

        state->Log();
    }

    for (TPair<FString, FSavedState>& Pair : objectToState)
    {
        LOG("%s", *Pair.Key)
        Pair.Value.Log();
    }
}

void USaveGameObject::Load(const UWorld* world)
{
    TArray<AActor*> savableActors;
    GetAllSavableActors(world, savableActors);

    for (TPair<FString, FSavedState>& Pair : objectToState)
    {
        LOG("Saved Actor: %s", *Pair.Key)
        Pair.Value.Log();
    }

    for (AActor* savableActor : savableActors)
    {
        FString actorName = savableActor->GetFullName();

        if (objectToState.Contains(actorName))
        {
            LOG("Loading Actor: %s", *actorName)
            Cast<ISavable>(savableActor)->SetState(&objectToState[actorName]);
        }
    }
}