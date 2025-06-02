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

void USaveGameObject::Save(UObject* worldContextObject)
{
    const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::Assert);

    LOG("SAVING")

    TArray<AActor*> savableActors;
    GetAllSavableActors(world, savableActors);

    for (AActor* savableActor : savableActors)
    {
        FString fullName = savableActor->GetFullName();

        FSavedState state;
        if (ISavableState* getState = Cast<ISavableState>(savableActor))
            state = getState->GetState();

        state.objectType      = savableActor->GetClass();
        state.objectTransform = savableActor->GetTransform();

        state.strings[FSavedState::NAME]     = savableActor->GetName(); 
        state.strings[FSavedState::FULLNAME] = fullName; 
        state.ints[FSavedState::RECREATE]    = Cast<ISavable>(savableActor)->recreate ? 1 : 0;

        objectToState.Add(fullName, state);

        state.Log();
    }
}

void USaveGameObject::Load(UObject* worldContextObject)
{
    LOG("LOADING")

    UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::Assert);

    TArray<AActor*> savableActors;
    GetAllSavableActors(world, savableActors);

    for (TPair<FString, FSavedState>& Pair : objectToState)
    {
        LOG("Saved Actor: %s", *Pair.Key)
        Pair.Value.Log();
    }

    for (AActor* savableActor : savableActors)
    {
        FString fullName = savableActor->GetFullName();

        LOG("Savable actor found: %s", *fullName)

        if (objectToState.Contains(fullName))
        {
            LOG("Loading Actor: %s", *fullName)

            savableActor->SetActorTransform(objectToState[fullName].objectTransform);

            if (ISavableState* setState = Cast<ISavableState>(savableActor))
                setState->SetState(objectToState[fullName]);

            objectToState.Remove(fullName);
        }
    }

    ULevel* level = world->GetCurrentLevel();

    TArray<FString> toRemove;

    for (TPair<FString, FSavedState>& Pair : objectToState)
    {
        if (!Pair.Value.ints[FSavedState::RECREATE])
            continue;
        
        LOG("Recreating Actor %s of type %s", *Pair.Key, *Pair.Value.objectType->GetName())

        FActorSpawnParameters spawnParams;
        spawnParams.Name = FName(Pair.Value.strings[FSavedState::NAME]);
        spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AActor* spawned = world->SpawnActor<AActor>(Pair.Value.objectType, Pair.Value.objectTransform, spawnParams);

        if (ISavableState* setStateOnSpawned = Cast<ISavableState>(spawned))
            setStateOnSpawned->SetState(Pair.Value);

        toRemove.Add(Pair.Key);
    }

    for (const FString& Key : toRemove)
        objectToState.Remove(Key);
    
    // do something with the remaining data, perhaps?
}