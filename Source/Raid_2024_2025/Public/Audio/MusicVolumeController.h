#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "MusicVolumeController.generated.h"

UCLASS()
class RAID_2024_2025_API AMusicVolumeController : public AActor
{
	GENERATED_BODY()
	
public:	
	AMusicVolumeController();

protected:
	virtual void BeginPlay() override;

public:	
	// Crossfade
	UFUNCTION(BlueprintCallable, Category = "Music")
	void CrossfadeMusic(float TargetNormalVolume, float TargetMuffledVolume, float Duration);

private:
	// Internal step update
	void UpdateCrossfadeStep();

	// Audio Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Music", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* NormalAudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Music", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* MuffledAudioComponent;

	// Sounds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Music", meta = (AllowPrivateAccess = "true"))
	USoundBase* NormalSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Music", meta = (AllowPrivateAccess = "true"))
	USoundBase* MuffledSound;

	// Crossfade data
	float InitialNormalVolume;
	float InitialMuffledVolume;
	float TargetNormalVolume;
	float TargetMuffledVolume;
	float CrossfadeDuration;
	float CrossfadeElapsed;
	float CrossfadeStepTime;

	FTimerHandle CrossfadeTimerHandle;
};