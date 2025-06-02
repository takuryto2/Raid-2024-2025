// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/SaveGameObject.h"

USaveGameObject::USaveGameObject()
{
    slotName = "save";
    slotIndex = 0;
}

void USaveGameObject::GetAllSavableActors(const UWorld* world, TArray<AActor*>& outSavable)
{
    UGameplayStatics::GetAllActorsWithInterface(world, USavable::StaticClass(), outSavable);
}

void USaveGameObject::Save(const UObject* worldContextObject)
{
    const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::Assert);

    LOG("SAVING")

    TArray<AActor*> savableActors;
    GetAllSavableActors(world, savableActors);

    for (AActor* savableActor : savableActors)
    {
        FString actorName = savableActor->GetFullName();
        LOG("%s", *actorName)
        
        FSavedState state = Cast<ISavable>(savableActor)->GetState();
        
        state.objectType = savableActor->GetClass();
        state.objectName = actorName;

        objectToState.Add(actorName, state);

        state.Log();
    }
}

void USaveGameObject::Load(const UObject* worldContextObject)
{
    LOG("LOADING")

    const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::Assert);

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

        LOG("Savable actor found: %s", *actorName)

        if (objectToState.Contains(actorName))
        {
            LOG("Loading Actor: %s", *actorName)
            Cast<ISavable>(savableActor)->SetState(objectToState[actorName]);
            objectToState.Remove(actorName);
        }
    }

    // ULevel* level = world->GetCurrentLevel();

    // TArray<FString> toRemove;

    // for (TPair<FString, FSavedState>& Pair : objectToState)
    // {
    //     if (!Pair.Value.recreateIfNotPresent)
    //         continue;
        
    //     LOG("Recreating Actor: %s", *Pair.Key)

    //     // assume each actor has the level as its outer
    //     AActor* spawned = NewObject<AActor>(level, Pair.Value.objectType, FName(Pair.Value.objectName));
    //     Cast<ISavable>(spawned)->SetState(Pair.Value);

    //     toRemove.Add(Pair.Key);
    // }

    // for (const FString& Key : toRemove)
    //     objectToState.Remove(Key);
    
    // do something with the remaining data, perhaps?
}