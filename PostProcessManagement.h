#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GarudaRift/ActorClasses/DayCycle.h"
#include "GarudaRift/SeasonEnum.h"
#include "PostProcessManagement.generated.h"

class APostProcessVolume;

UCLASS()
class GARUDARIFT_API APostProcessManagement : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APostProcessManagement();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Timekeeping
	void UpdatePostProcess(const FString& CurrentDayPhase);

	// Transition Management
	void ShiftSceneLightingDynamics(float DeltaTime);

	APostProcessVolume* SceneLighting;

	// Arrays
	TArray<UTextureCube*> DayPhaseTextures;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Private Timekeeping
	FString CurrentDayPhaseState;
	FString LastDayPhaseState;
	FTimerHandle LightingTimerHandle;
	float TimerValue;
	float TotalTransitionTime;
	float LerpSpeed;
	bool bNewPhaseStart;
	bool bIsLerping = false;

	UTexture2D* BokehShapeTexture;
};
