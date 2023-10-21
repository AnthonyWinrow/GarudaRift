#include "GarudaRift/ActorClasses/PostProcessManagement.h"
#include "Engine/PostProcessVolume.h"
#include "EngineUtils.h"
	
DECLARE_LOG_CATEGORY_EXTERN(LogPostProcessManagement, Log, All);
DEFINE_LOG_CATEGORY(LogPostProcessManagement);

// Sets default values
APostProcessManagement::APostProcessManagement()
	: DawnLighting(nullptr), SunriseLighting(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TransitionSpeed = 0.0f;
	bNewPhaseStart = false;
}

// Called when the game starts or when spawned
void APostProcessManagement::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<APostProcessVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->ActorHasTag(FName("DawnLighting")))
		{
			DawnLighting = *ActorItr;
		}
		else if (ActorItr->ActorHasTag(FName("SunriseLighting")))
		{
			SunriseLighting = *ActorItr;
		}

		UE_LOG(LogPostProcessManagement, Log, TEXT("Found Post Process Volume =: %s"), *ActorItr->GetName());
	}

	if (DawnLighting && SunriseLighting)
	{
		DawnLighting->bEnabled = true;
		SunriseLighting->bEnabled = true;

		DawnLighting->BlendWeight = 0.0f;
		SunriseLighting->BlendWeight = 0.0f;
	}
	else
	{
		UE_LOG(LogPostProcessManagement, Error, TEXT("Could not find one or more Post Process Volumes. Check their names."));
	}
}

// Called every frame
void APostProcessManagement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TransitionPostProcess(DeltaTime);
}

void APostProcessManagement::UpdatePostProcess(const FString& CurrentDayPhase)
{
	CurrentDayPhaseState = CurrentDayPhase;

	UE_LOG(LogPostProcessManagement, Log, TEXT("CurrentDayPhaseState Updated =: %s"), *CurrentDayPhaseState); 
}

void APostProcessManagement::TransitionPostProcess(float DeltaTime)
{
	// Check for day phase changes
	if (CurrentDayPhaseState != LastDayPhaseState)
	{
		TransitionSpeed = 0.0f;
		bNewPhaseStart = true;
		LastDayPhaseState = CurrentDayPhaseState;
	}

	// Incremental transition speed
	TransitionSpeed += DeltaTime * 0.1f;
	TransitionSpeed = FMath::Clamp(TransitionSpeed, 0.0f, 1.0f);

	if (!DawnLighting || !SunriseLighting)
	{
		UE_LOG(LogPostProcessManagement, Log, TEXT("Post Process Volumes Not Found, Unable to Transition"));
		return;
	}

	if (CurrentDayPhaseState == "Dawn")
	{
		if (bNewPhaseStart)
		{
			DawnLighting->BlendWeight = 0.0f;
			SunriseLighting->BlendWeight = 0.0f;
			bNewPhaseStart = false;
		}

		DawnLighting->BlendWeight = FMath::Lerp(0.0f, 1.0f, TransitionSpeed);
		SunriseLighting->BlendWeight = FMath::Lerp(0.0f, 0.0f, TransitionSpeed);
	}
	else if (CurrentDayPhaseState == "Sunrise")
	{
		if (bNewPhaseStart)
		{
			DawnLighting->BlendWeight = 0.0f;
			SunriseLighting->BlendWeight = 0.0f;
			bNewPhaseStart = false;
		}

		DawnLighting->BlendWeight = FMath::Lerp(1.0f, 0.0f, TransitionSpeed);
		SunriseLighting->BlendWeight = FMath::Lerp(0.0f, 1.0f, TransitionSpeed);
	}
	else if (CurrentDayPhaseState == "Morning")
	{
		if (bNewPhaseStart)
		{
			DawnLighting->BlendWeight = 1.0f;
			SunriseLighting->BlendWeight = 1.0f;
			bNewPhaseStart = false;
		}

		SunriseLighting->BlendWeight = FMath::Lerp(1.0f, 0.0f, TransitionSpeed);
	}
	else if (CurrentDayPhaseState == "Late Morning")
	{
		if (bNewPhaseStart)
		{
			SunriseLighting->BlendWeight = 0.0f;
			bNewPhaseStart = false;
		}
	}
	else
	{
		UE_LOG(LogPostProcessManagement, Error, TEXT("Post Process Volumes Not Found, Unable to Transition"));
		return;
	}
}
