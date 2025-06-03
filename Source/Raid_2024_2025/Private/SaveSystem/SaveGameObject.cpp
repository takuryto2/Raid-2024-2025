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

    SaveObjects(savableActors, worldContextObject);
}

void USaveGameObject::Load(UObject* worldContextObject)
{
    LOG("LOADING")

    UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::Assert);

    TArray<AActor*> savableActors;
    GetAllSavableActors(world, savableActors);

    LoadObjects(savableActors, worldContextObject);
}

void USaveGameObject::SaveObjects(const TArray<AActor*> actors, UObject* worldContextObject)
{
    for (AActor* savableActor : actors)
    {
        FString fullName = savableActor->GetFullName();
        LOG("Savable actor found: %s", *fullName)

        FSavedState state;
        if (savableActor->Implements<USavableState>())
        {
            state = ISavableState::Execute_GetState(savableActor);
            LOG("Saving state of actor %s", *fullName)
        }

        state.objectType      = savableActor->GetClass();
        state.objectTransform = savableActor->GetTransform();

        state.strings.Add(FSavedState::NAME,            savableActor->GetName()); 
        state.strings.Add(FSavedState::FULLNAME,        fullName); 
        state.ints.Add(   FSavedState::SHOULD_RECREATE, ISavable::Execute_ShouldRecreate(savableActor) ? 1 : 0);

        state.Log();

        objectToState.Add(fullName, state);
    }
}

void USaveGameObject::LoadObjects(const TArray<AActor*> actors, UObject* worldContextObject)
{
    UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::Assert);

    for (TPair<FString, FSavedState>& Pair : objectToState)
    {
        LOG("Saved Actor: %s", *Pair.Key)
        Pair.Value.Log();
    }
    LOG("=================================")

    for (AActor* savableActor : actors)
    {
        FString fullName = savableActor->GetFullName();
        LOG("Savable actor found: %s", *fullName)

        if (objectToState.Contains(fullName))
        {
            LOG("Loading actor: %s", *fullName)

            savableActor->SetActorTransform(objectToState[fullName].objectTransform);

            if (savableActor->Implements<USavableState>())
            {
                ISavableState::Execute_SetState(savableActor, objectToState[fullName]);
                LOG("Setting state of actor %s", *fullName)
            }

            objectToState.Remove(fullName);
        }
    }

    ULevel* level = world->GetCurrentLevel();

    TArray<FString> toRemove;

    for (TPair<FString, FSavedState>& Pair : objectToState)
    {
        if (!Pair.Value.ints[FSavedState::SHOULD_RECREATE])
            continue;
        
        LOG("Recreating actor %s of type %s", *Pair.Key, *Pair.Value.objectType->GetName())

        FActorSpawnParameters spawnParams;
        spawnParams.Name = FName(Pair.Value.strings[FSavedState::NAME]);
        spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AActor* spawned = world->SpawnActor<AActor>(Pair.Value.objectType, Pair.Value.objectTransform, spawnParams);

        if (spawned->Implements<USavableState>())
        {
            ISavableState::Execute_SetState(spawned, Pair.Value);
            LOG("Setting state of recreated actor %s", *Pair.Value.strings[FSavedState::FULLNAME])
        }

        toRemove.Add(Pair.Key);
    }

    for (const FString& Key : toRemove)
        objectToState.Remove(Key);
    
    // do something with the remaining data, perhaps?
}