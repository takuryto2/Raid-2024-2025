// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem/SaveGameObject.h"

void USaveGameObject::GetAllSavableActors(TArray<ISavable*>& outSavable)
{
    TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsWithInterface(GWorld, USavable::StaticClass(), actors);

    for (AActor* actor : actors)
    {
        if (!actor->Implements<USavable>())
            continue;
        
        ISavable* savableActor = Cast<ISavable>(actor);

        if (!savableActor)
            continue;
        
        outSavable.Add(savableActor);
    }
}

void USaveGameObject::Save()
{
    TArray<ISavable*> savableActors;
    USaveGameObject::GetAllSavableActors(savableActors);

    for (ISavable* savableActor : savableActors)
    {
        FString actorName = Cast<AActor>(savableActor)->GetFullName();
        FSavedState* state = savableActor->GetState();
        objectToState.Add(actorName, *state);
    }
}

void USaveGameObject::Load()
{
    TArray<ISavable*> savableActors;
    USaveGameObject::GetAllSavableActors(savableActors);

    for (ISavable* savableActor : savableActors)
    {
        FString actorName = Cast<AActor>(savableActor)->GetFullName();

        if (objectToState.Contains(actorName))
        {
            savableActor->SetState(&objectToState[actorName]);
        }
    }
}