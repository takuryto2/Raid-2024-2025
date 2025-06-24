#include "Audio/AudioPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void UAudioPlayer::Play3DSound(UObject* WorldContextObject, USoundBase* Sound, FVector Location)
{
    if (Sound && WorldContextObject)
    {
        UGameplayStatics::PlaySoundAtLocation(WorldContextObject, Sound, Location);
    }
}
