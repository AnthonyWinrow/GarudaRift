#include "GarudaRift/ActorClasses/DayCycle.h"
#include "SunPosition.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDayCycle, Log, All);
DEFINE_LOG_CATEGORY(LogDayCycle);

// Sets default values
ADayCycle::ADayCycle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Sunlight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sunlight"));
	Sunlight->SetupAttachment(Root);

	Latitude = 0.0f;
	Longitude = 0.0f;
	Timezone = 0.0f;
	bDaylightSavings = false;
	Year = 2023;
	Month = 10;
	Day = 8;
	Hours = 8;
	Minutes = 0;
	Seconds = 0;
}

// Called when the game starts or when spawned
void ADayCycle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADayCycle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSunPosition();
}

void ADayCycle::UpdateSunPosition()
{
	FSunPositionData SunData;

	USunPositionFunctionLibrary::GetSunPosition(Latitude, Longitude, Timezone, bDaylightSavings, Year, Month, Day, Hours, Minutes, Seconds, SunData);

	Sunlight->SetWorldRotation(FRotator(SunData.Elevation, SunData.Azimuth, 0));
}
