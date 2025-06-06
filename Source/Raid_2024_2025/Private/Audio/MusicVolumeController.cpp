#include "Audio/MusicVolumeController.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "Engine/World.h"

// Sets default values
AMusicVolumeController::AMusicVolumeController()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root Component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Normal Audio
	NormalAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("NormalAudioComponent"));
	NormalAudioComponent->SetupAttachment(RootComponent);
	NormalAudioComponent->bAutoActivate = false;

	// Muffled Audio
	MuffledAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MuffledAudioComponent"));
	MuffledAudioComponent->SetupAttachment(RootComponent);
	MuffledAudioComponent->bAutoActivate = false;

	// Initial values
	CrossfadeDuration = 0.f;
	CrossfadeElapsed = 0.f;
	CrossfadeStepTime = 0.05f;
}

void AMusicVolumeController::BeginPlay()
{
	Super::BeginPlay();

	if (NormalAudioComponent)
	{
		NormalAudioComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		if (NormalSound)
		{
			NormalAudioComponent->SetSound(NormalSound);
			NormalAudioComponent->Play();
		}
	}

	if (MuffledAudioComponent)
	{
		MuffledAudioComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		if (MuffledSound)
		{
			MuffledAudioComponent->SetSound(MuffledSound);
			MuffledAudioComponent->Play();
		}
	}

	NormalAudioComponent->SetVolumeMultiplier(1.f);
	MuffledAudioComponent->SetVolumeMultiplier(0.f);
}

void AMusicVolumeController::CrossfadeMusic(float InTargetNormalVolume, float InTargetMuffledVolume, float Duration)
{
	if (!NormalAudioComponent || !MuffledAudioComponent)
		return;

	GetWorld()->GetTimerManager().ClearTimer(CrossfadeTimerHandle);

	InitialNormalVolume = NormalAudioComponent->VolumeMultiplier;
	InitialMuffledVolume = MuffledAudioComponent->VolumeMultiplier;

	TargetNormalVolume = InTargetNormalVolume;
	TargetMuffledVolume = InTargetMuffledVolume;

	CrossfadeDuration = FMath::Max(Duration, KINDA_SMALL_NUMBER);
	CrossfadeElapsed = 0.f;

	const int32 NumSteps = FMath::CeilToInt(CrossfadeDuration / CrossfadeStepTime);
	CrossfadeStepTime = CrossfadeDuration / NumSteps;

	GetWorld()->GetTimerManager().SetTimer(
		CrossfadeTimerHandle,
		this,
		&AMusicVolumeController::UpdateCrossfadeStep,
		CrossfadeStepTime,
		true
	);
}

void AMusicVolumeController::UpdateCrossfadeStep()
{
	CrossfadeElapsed += CrossfadeStepTime;

	float Alpha = FMath::Clamp(CrossfadeElapsed / CrossfadeDuration, 0.f, 1.f);

	float NewNormal = FMath::Lerp(InitialNormalVolume, TargetNormalVolume, Alpha);
	float NewMuffled = FMath::Lerp(InitialMuffledVolume, TargetMuffledVolume, Alpha);

	if (NormalAudioComponent)
		NormalAudioComponent->SetVolumeMultiplier(NewNormal);
	if (MuffledAudioComponent)
		MuffledAudioComponent->SetVolumeMultiplier(NewMuffled);

	if (Alpha >= 1.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CrossfadeTimerHandle);
	}
}
