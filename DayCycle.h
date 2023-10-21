#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SunPosition.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SceneComponent.h"
#include "GarudaRift/SeasonEnum.h"
#include "DayCycle.generated.h"

class APostProcessManagement;

UCLASS()
class GARUDARIFT_API ADayCycle : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADayCycle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Control")
	float TimeScale;

	// Sun position parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	float Latitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	float Longitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	int32 Timezone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	bool bDaylightSavings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	int32 Year;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	int32 Month;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	int32 Day;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	int32 Hours;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	int32 Minutes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	int32 Seconds;

	// Calendar Year
	int32 DaysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (HideCategories = "Light, Rendering, LOD, TextureStreaming"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (EditCondition = "false"))
	UDirectionalLightComponent* Sunlight;

	UPROPERTY(VisibleAnywhere)
	USkyLightComponent* Skylight;

	// External Classes
	APostProcessManagement* PostProcessManagement;

	void UpdatePostProcessManagement();

	// Lighting Updates
	void SunlightIntensity(float DeltaTime);
	void SunLightColor(float DeltaTime);
	void SkylightIntensity(float DeltaTime);
	void UpdateSunPosition();
	void SunlightBloomScale(float DeltaTime);
	void SunlightBloomTint(float DeltaTime);

	float NormalizedTime;
	float NormalizedTimeIntensity;
	float NormalizedTimeBloomScale;
	float NormalizedTimeBloomTint;

	bool bInterpolate;
	bool bInterpolateIntensity;
	bool bInterpolateBloomScale;
	bool bInterpolateBloomTint;

	int CurrentPhaseIndex;
	int NextPhaseIndex;
	int CurrentIntensityIndex;
	int NextIntensityIndex;
	int CurrentSkylightIntensityIndex;
	int NextSkylightIntensityIndex;
	int CurrentBloomScaleIndex;
	int NextBloomScaleIndex;
	int CurrentBloomTintIndex;
	int NextBloomTintIndex;

	TArray<FLinearColor> PhaseColors;
	TArray<FLinearColor> BloomTints;
	TArray<float> PhaseIntensities;
	TArray<float> SkylightPhaseIntensities;
	TArray<float> BloomScales;

	// Editor Updates
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

	// Pointers
	class USunPosition* SunPosition;

	// Day Phases
	FString Dawn = "Dawn";
	FString Sunrise = "Sunrise";
	FString Morning = "Morning";
	FString LateMorning = "Late Morning";
	FString Noon = "Noon";
	FString EarlyAfternoon = "Early Afternoon";
	FString LateAfternoon = "Late Afternoon";
	FString Sunset = "Sunset";
	FString Twilight = "Twilight";
	FString Evening = "Evening";
	FString Night = "Night";
	FString Midnight = "Midnight";
	FString LateNight = "Late Night";
	FString CurrentDayPhase;
	FString LastDayPhase;
	FString LastIntensityPhase;
	FString LastSkylightIntensityPhase;
	FString LastBloomPhase;
	FString LastBloomTintPhase;

	FDateTime DawnTime;
	FDateTime SunriseTime;
	FDateTime MorningTime;
	FDateTime LateMorningTime;
	FDateTime NoonTime;
	FDateTime EarlyAfternoonTime;
	FDateTime LateAfternoonTime;
	FDateTime SunsetTime;
	FDateTime TwilightTime;
	FDateTime EveningTime;
	FDateTime NightTime;
	FDateTime MidnightTime;
	FDateTime LateNightTime;
	FDateTime CurrentTime;

	void UpdateDayPhase();
	void UpdateSeason();
	void UpdateSpring();
	void UpdateSummer();
	void UpdateAutumn();
	void UpdateWinter();

	ESeason CurrentSeason;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
