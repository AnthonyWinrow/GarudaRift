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
	
	UpdateSeason();
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

	CurrentTime = FDateTime::Now();

	UpdateSeason();
	if (CurrentSeason == Spring)
	{
		UpdateSpring();
	}
	else if (CurrentSeason == Summer)
	{
		UpdateSummer();
	}
	else if (CurrentSeason == Autumn)
	{
		UpdateAutumn();
	}
	else if (CurrentSeason == Winter)
	{
		UpdateWinter();
	}

	UpdateDayPhase();
	UpdateSunPosition();
}

void ADayCycle::UpdateSunPosition()
{
	FSunPositionData SunData;

	USunPositionFunctionLibrary::GetSunPosition(Latitude, Longitude, Timezone, bDaylightSavings, Year, Month, Day, Hours, Minutes, Seconds, SunData);

	Sunlight->SetWorldRotation(FRotator(SunData.Elevation, SunData.Azimuth, 0));
}

void ADayCycle::UpdateSeason()
{
	if (Month >= 3 && Month <= 5)
	{
		CurrentSeason = Spring;
	}
	else if (Month >= 6 && Month <= 8)
	{
		CurrentSeason = Summer;
	}
	else if (Month >= 9 && Month <= 10)
	{
		CurrentSeason = Autumn;
	}
	else if (Month >= 11 && Month <= 3)
	{
		CurrentSeason = Winter;
	}
}

void ADayCycle::UpdateSpring()
{
	DawnTime = FDateTime(Year, Month, Day, 4, 0, 0);
	SunriseTime = FDateTime(Year, Month, Day, 6, 0, 0);
	MorningTime = FDateTime(Year, Month, Day, 8, 0, 0);
	LateMorningTime = FDateTime(Year, Month, Day, 10, 0, 0);
	NoonTime = FDateTime(Year, Month, Day, 12, 0, 0);
	EarlyAfternoonTime = FDateTime(Year, Month, Day, 14, 0, 0);
	LateAfternoonTime = FDateTime(Year, Month, Day, 16, 0, 0);
	SunsetTime = FDateTime(Year, Month, Day, 18, 0, 0);
	TwilightTime = FDateTime(Year, Month, Day, 20, 0, 0);
	EveningTime = FDateTime(Year, Month, Day, 22, 0, 0);
	NightTime = FDateTime(Year, Month, Day, 0, 0, 0);
	MidnightTime = FDateTime(Year, Month, Day, 2, 0, 0);
	LateNightTime = FDateTime(Year, Month, Day, 3, 0, 0);
}

void ADayCycle::UpdateSummer()
{
	DawnTime = FDateTime(Year, Month, Day, 4, 0, 0);
	SunriseTime = FDateTime(Year, Month, Day, 5, 0, 0);
	MorningTime = FDateTime(Year, Month, Day, 7, 0, 0);
	LateMorningTime = FDateTime(Year, Month, Day, 9, 0, 0);
	NoonTime = FDateTime(Year, Month, Day, 12, 0, 0);
	EarlyAfternoonTime = FDateTime(Year, Month, Day, 14, 0, 0);
	LateAfternoonTime = FDateTime(Year, Month, Day, 16, 0, 0);
	SunsetTime = FDateTime(Year, Month, Day, 20, 0, 0);
	TwilightTime = FDateTime(Year, Month, Day, 21, 0, 0);
	EveningTime = FDateTime(Year, Month, Day, 22, 0, 0);
	NightTime = FDateTime(Year, Month, Day, 23, 0, 0);
	MidnightTime = FDateTime(Year, Month, Day, 0, 0, 0);
	LateNightTime = FDateTime(Year, Month, Day, 1, 0, 0);
}

void ADayCycle::UpdateAutumn()
{
	DawnTime = FDateTime(Year, Month, Day, 6, 0, 0);
	SunriseTime = FDateTime(Year, Month, Day, 7, 0, 0);
	MorningTime = FDateTime(Year, Month, Day, 9, 0, 0);
	LateMorningTime = FDateTime(Year, Month, Day, 10, 0, 0);
	NoonTime = FDateTime(Year, Month, Day, 12, 0, 0);
	EarlyAfternoonTime = FDateTime(Year, Month, Day, 14, 0, 0);
	LateAfternoonTime = FDateTime(Year, Month, Day, 16, 0, 0);
	SunsetTime = FDateTime(Year, Month, Day, 18, 0, 0);
	TwilightTime = FDateTime(Year, Month, Day, 19, 0, 0);
	EveningTime = FDateTime(Year, Month, Day, 20, 0, 0);
	NightTime = FDateTime(Year, Month, Day, 22, 0, 0);
	MidnightTime = FDateTime(Year, Month, Day, 0, 0, 0);
	LateNightTime = FDateTime(Year, Month, Day, 2, 0, 0);
}

void ADayCycle::UpdateWinter()
{
	DawnTime = FDateTime(Year, Month, Day, 7, 0, 0);
	SunriseTime = FDateTime(Year, Month, Day, 8, 0, 0);
	MorningTime = FDateTime(Year, Month, Day, 9, 0, 0);
	LateMorningTime = FDateTime(Year, Month, Day, 10, 0, 0);
	NoonTime = FDateTime(Year, Month, Day, 12, 0, 0);
	EarlyAfternoonTime = FDateTime(Year, Month, Day, 13, 0, 0);
	LateAfternoonTime = FDateTime(Year, Month, Day, 15, 0, 0);
	SunsetTime = FDateTime(Year, Month, Day, 16, 0, 0);
	TwilightTime = FDateTime(Year, Month, Day, 17, 0, 0);
	EveningTime = FDateTime(Year, Month, Day, 18, 0, 0);
	NightTime = FDateTime(Year, Month, Day, 20, 0, 0);
	MidnightTime = FDateTime(Year, Month, Day, 22, 0, 0);
	LateNightTime = FDateTime(Year, Month, Day, 0, 0, 0);
}

void ADayCycle::UpdateDayPhase()
{
	if (CurrentTime >= DawnTime && CurrentTime < SunriseTime)
	{
		CurrentDayPhase = "Dawn";
	}
	else if (CurrentTime >= SunriseTime && CurrentTime < MorningTime)
	{
		CurrentDayPhase = "Sunrise";
	}
	else if (CurrentTime >= MorningTime && CurrentTime < LateMorningTime)
	{
		CurrentDayPhase = "Morning";
	}
	else if (CurrentTime >= LateMorningTime && CurrentTime < NoonTime)
	{
		CurrentDayPhase = "Late Morning";
	}
	else if (CurrentTime >= NoonTime && CurrentTime < EarlyAfternoonTime)
	{
		CurrentDayPhase = "Noon";
	}
	else if (CurrentTime >= EarlyAfternoonTime && CurrentTime < LateAfternoonTime)
	{
		CurrentDayPhase = "Early Afternoon";
	}
	else if (CurrentTime >= LateAfternoonTime && CurrentTime < SunsetTime)
	{
		CurrentDayPhase = "Late Afternoon";
	}
	else if (CurrentTime >= SunsetTime && CurrentTime < TwilightTime)
	{
		CurrentDayPhase = "Sunset";
	}
	else if (CurrentTime >= TwilightTime && CurrentTime < EveningTime)
	{
		CurrentDayPhase = "Twilight";
	}
	else if (CurrentTime >= EveningTime && CurrentTime < NightTime)
	{
		CurrentDayPhase = "Evening";
	}
	else if (CurrentTime >= NightTime && CurrentTime < MidnightTime)
	{
		CurrentDayPhase = "Midnight";
	}
	else
	{
		CurrentDayPhase = "Late Night";
	}
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
