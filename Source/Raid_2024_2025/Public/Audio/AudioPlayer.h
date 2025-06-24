#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AudioPlayer.generated.h"

UCLASS()
class RAID_2024_2025_API UAudioPlayer : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Audio")
    static void Play3DSound(UObject* WorldContextObject, USoundBase* Sound, FVector Location);
};
