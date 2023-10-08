#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Engine/World.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Engine/ExponentialHeightFog.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/CoreUObject/Public/UObject/ObjectMacros.h"
#include "E:/EpicGames/UE_5.2/Engine/Plugins/Runtime/SunPosition/Source/SunPosition/Public/SunPosition.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Engine/PostProcessVolume.h"
#include "SunPositionController.generated.h"

USTRUCT()
struct FSunPositionTime
{
	GENERATED_BODY()

		FSunPositionData SunPosition;
		float Time;
};

UCLASS()
class GARUDARIFT_API ASunPositionController : public AActor
{
	GENERATED_BODY()

public:
	ASunPositionController();

	// Function to adjust the sun's intensity based on time of day
	void AdjustSunIntensity();

	// Function to adjust the sun's color based on time of day
	void AdjustSunColor();

	// Function to adjust the skylight's intensity based on time of day
	void AdjustSkyLightIntensity();

	// Function to adjust the skylight's color based on time of day
	void AdjustSkyLightColor();

	// Function to adjust the volumetric fog density based on time of day
	void AdjustFogExtinctionScale();

	// Function to adjust the post processing volume's settings
	void AdjustPostProcessSettings();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	float PreviousMinBrightness;
	float PreviousMaxBrightness;

	UFUNCTION(BlueprintCallable, Category = "Sun Position")
		void GetSunPosition();
	
	UFUNCTION(BlueprintCallable, Category = "Sun Position")
	void StartDay();

	void UpdateSunPosition();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sun Position")
		UDirectionalLightComponent* SunLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sun Position")
		USkyLightComponent* SkyLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Sun Position")
		AExponentialHeightFog* FogActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sun Position")
		float CurrentTimeOfDay;

	UPROPERTY(EditAnywhere, Category = "Sun Position")
		float TimeScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sun Positoin")
		float Latitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
		float Longitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
		int32 TimeZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
		bool bIsDaylightSavingTime;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
		FSunPositionData SunPositionData;

	UPROPERTY(EditAnywhere, Category = "Sun Position")
		FColor SunriseColor;

	UPROPERTY(EditAnywhere, Category = "Sun Position")
		FColor MorningColor;

	UPROPERTY(EditAnywhere, Category = "Sun Position")
		FColor AfternoonColor;

	UPROPERTY(EditAnywhere, Category = "Sun Position")
		FColor EveningColor;

	UPROPERTY(EditAnywhere, Category = "Sun Position")
		FColor NightColor;

	TArray<FSunPositionTime> SunPositions;

	APostProcessVolume* PostProcessVolume;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
