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
	void ShiftSceneLightingDynamics();

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
	bool bNewPhaseStart;

	UTexture2D* BokehShapeTexture;
};
