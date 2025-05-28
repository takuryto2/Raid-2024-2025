// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem/SaveGameObject.h"










void USaveGameObject::GetAllSavableActors(TArray<TScriptInterface<ISavable>>& outSavable)
{
    TArray<AActor*> actors;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USavable::StaticClass(), actors);

    for (AActor* actor : actors)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, actor->GetName());
        outSavable.Add(TScriptInterface<ISavable>(actor));
    }
}


















void USaveGameObject::Save()
{
    TArray<TScriptInterface<ISavable>> savableActors;
    GetAllSavableActors(savableActors);

    for (const TScriptInterface<ISavable>& savableActor : savableActors)
    {
        AActor* actor = Cast<AActor>(savableActor.GetObject());

        if (!&actor)
            continue;

        FString actorName = actor->GetFullName();
        FSavedState* state = savableActor->GetState();
        objectToState.Add(actorName, *state);
    }
}

void USaveGameObject::Load()
{
    TArray<TScriptInterface<ISavable>> savableActors;
    GetAllSavableActors(savableActors);

    for (const TScriptInterface<ISavable>& savableActor : savableActors)
    {
        AActor* actor = Cast<AActor>(savableActor.GetObject());

        if (!&actor)
            continue;

        FString actorName = actor->GetFullName();

        if (objectToState.Contains(actorName))
        {
            savableActor->SetState(&objectToState[actorName]);
        }
    }
}