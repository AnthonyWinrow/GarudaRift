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

	TimeScale = 1.0f;
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
	AngleIncrement = 0.1f;

	UE_LOG(LogTemp, Error, TEXT("Constructor Called_daycycle"));
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

	if (Month == 2 && Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0))
	{
		DaysInMonth[1] = 29;
	}
	else
	{
		DaysInMonth[1] = 28;
	}

	if (Month == 3 && Day >= 8 && Day <= 14 && Hours == 2 && !bDaylightSavings)
	{
		bDaylightSavings = true;
		Timezone--;
	}
	else if (Month == 11 && Day >= 1 && Day <= 7 && Hours == 1 && bDaylightSavings)
	{
		bDaylightSavings = false;
		Timezone++;
	}

	Seconds += DeltaTime * TimeScale;
	if (Seconds >= 60)
	{
		Minutes++;
		Seconds = 0;
	}
	if (Minutes >= 60)
	{
		Hours++;
		Minutes = 0;
	}
	if (Hours >= 24)
	{
		Day++;
		Hours = 0;
	}

	UpdateSunPosition();
}

void ADayCycle::UpdateSunPosition()
{
	FSunPositionData SunData;

	USunPositionFunctionLibrary::GetSunPosition(Latitude, Longitude, Timezone, bDaylightSavings, Year, Month, Day, Hours, Minutes, Seconds, SunData);

	Sunlight->SetWorldRotation(FRotator(SunData.Elevation, SunData.Azimuth, 0));
}

void ADayCycle::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ADayCycle, Latitude) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayCycle, Longitude) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayCycle, Timezone) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayCycle, bDaylightSavings) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayCycle, Year) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayCycle, Month) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayCycle, Day) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayCycle, Hours) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayCycle, Minutes) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ADayCycle, Seconds))
	{
		UpdateSunPosition();
	}
}
