// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SunPosition.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SceneComponent.h"
#include "DayCycle.generated.h"

UCLASS()
class GARUDARIFT_API ADayCycle : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADayCycle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (HideCategories = "Light, Rendering, LOD, TextureStreaming"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (EditCondition = "false"))
	UDirectionalLightComponent* Sunlight;

	// Lighting Updates
	void UpdateSunPosition();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
