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
	DawnTime = FDateTime(2023, 10, 9, 6, 0, 0);
	SunriseTime = FDateTime(2023, 10, 9, 7, 0, 0);
	MorningTime = FDateTime(2023, 10, 9, 8, 0, 0);
	LateMorningTime = FDateTime(2023, 10, 9, 10, 0, 0);
	NoonTime = FDateTime(2023, 10, 9, 12, 0, 0);
	EarlyAfternoonTime = FDateTime(2023, 10, 9, 14, 0, 0);
	LateAfternoonTime = FDateTime(2023, 10, 9, 16, 0, 0);
	SunsetTime = FDateTime(2023, 10, 9, 18, 0, 0);
	TwilightTime = FDateTime(2023, 10, 9, 19, 0, 0);
	EveningTime = FDateTime(2023, 10, 9, 20, 0, 0);
	NightTime = FDateTime(2023, 10, 9, 21, 0, 0);
	MidnightTime = FDateTime(2023, 10, 9, 0, 0, 0);

	Dawn = "Dawn";
	Sunrise = "Sunrise";
	Morning = "Morning";
	LateMorning = "Late Morning";
	Noon = "Noon";
	EarlyAfternoon = "Early Afternoon";
	LateAfternoon = "Late Afternoon";
	Sunset = "Sunset";
	Twilight = "Twilight";
	Evening = "Evening";
	Night = "Night";
	Midnight = "Midnight";
	LateNight = "Late Night";

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

	UE_LOG(LogDayCycle, Error, TEXT("CurrentTime =: %s"), *CurrentTime.ToString());

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
	SunlightIntensity();
	UpdateSunPosition();

	UE_LOG(LogDayCycle, Error, TEXT("CurrentDayPhase =: %s"), *CurrentDayPhase);
}

void ADayCycle::SunlightIntensity()
{
	if (CurrentDayPhase == "Dawn" || CurrentDayPhase == "Twilight")
	{
		Sunlight->Intensity = 10.0f;
	}
	else if (CurrentDayPhase == "Sunrise" || CurrentDayPhase == "Sunset")
	{
		Sunlight->Intensity = 400.0f;
	}
	else if (CurrentDayPhase == "Noon")
	{
		Sunlight->Intensity = 120000.0f;
	}
	else
	{
		Sunlight->Intensity = 20000.0f;
	}
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
	UE_LOG(LogDayCycle, Error, TEXT("UpdateDayPhase Called"));
	UE_LOG(LogDayCycle, Error, TEXT("CurrentTime =: %s"), *CurrentTime.ToString());

	int32 CurrentHour = CurrentTime.GetHour();
	int32 CurrentMinute = CurrentTime.GetMinute();

	UE_LOG(LogDayCycle, Error, TEXT("CurrentHour =: %d, CurrentMinute =: %ds:"), CurrentHour, CurrentMinute);

	int32 CurrentSecond = CurrentTime.GetSecond();

	if (CurrentHour >= DawnTime.GetHour() && CurrentHour < SunriseTime.GetHour())
	{
		CurrentDayPhase = Dawn;
		UE_LOG(LogDayCycle, Error, TEXT("CurrentDayPhase Set to Dawn"));
	}
	else if (CurrentHour >= SunriseTime.GetHour() && CurrentHour < MorningTime.GetHour())
	{
		CurrentDayPhase = Sunrise;
		UE_LOG(LogDayCycle, Error, TEXT("CurrentDayPhase Set to Sunrise"));
	}
	else if (CurrentHour >= MorningTime.GetHour() && CurrentHour < LateMorningTime.GetHour())
	{
		CurrentDayPhase = Morning;
	}
	else if (CurrentHour >= LateMorningTime.GetHour() && CurrentHour < NoonTime.GetHour())
	{
		CurrentDayPhase = LateMorning;
	}
	else if (CurrentHour >= NoonTime.GetHour() && CurrentHour < EarlyAfternoonTime.GetHour())
	{
		CurrentDayPhase = Noon;
	}
	else if (CurrentHour >= EarlyAfternoonTime.GetHour() && CurrentHour < LateAfternoonTime.GetHour())
	{
		CurrentDayPhase = EarlyAfternoon;
	}
	else if (CurrentHour >= LateAfternoonTime.GetHour() && CurrentHour < SunsetTime.GetHour())
	{
		CurrentDayPhase = LateAfternoon;
	}
	else if (CurrentHour >= SunsetTime.GetHour() && CurrentHour < TwilightTime.GetHour())
	{
		CurrentDayPhase = Sunset;
	}
	else if (CurrentHour >= TwilightTime.GetHour() && CurrentHour < EveningTime.GetHour())
	{
		CurrentDayPhase = Twilight;
	}
	else if (CurrentHour >= EveningTime.GetHour() && CurrentHour < MidnightTime.GetHour())
	{
		CurrentDayPhase = Evening;
		UE_LOG(LogDayCycle, Error, TEXT("CurrentDayPhase Set to Evening"));
	}
	else if (CurrentHour >= MidnightTime.GetHour() && CurrentHour < LateNightTime.GetHour())
	{
		CurrentDayPhase = Midnight;
	}
	else
	{
		CurrentDayPhase = LateNight;
		UE_LOG(LogDayCycle, Error, TEXT("CurrentDayPhase Set to LateNight"));
	}

	UE_LOG(LogDayCycle, Error, TEXT("DawnTime =: %s, Hour =: %d, Minute =: %d"), *DawnTime.ToString(), DawnTime.GetHour(), DawnTime.GetMinute());
	UE_LOG(LogDayCycle, Error, TEXT("SunriseTime =: %s, Hour =: %d, Minute =: %d"), *SunriseTime.ToString(), SunriseTime.GetHour(), SunriseTime.GetMinute());
	UE_LOG(LogDayCycle, Error, TEXT("MorningTime =: %s, Hour =: %d, Minute =: %d"), *MorningTime.ToString(), MorningTime.GetHour(), MorningTime.GetMinute());
	UE_LOG(LogDayCycle, Error, TEXT("LateMorningTime =: %s, Hour =: %d, Minute =: %d"), *LateMorningTime.ToString(), LateMorningTime.GetHour(), LateMorningTime.GetMinute());
	UE_LOG(LogDayCycle, Error, TEXT("NoonTime =: %s, Hour =: %d, Minute =: %d"), *NoonTime.ToString(), NoonTime.GetHour(), NoonTime.GetMinute());
	UE_LOG(LogDayCycle, Error, TEXT("EarlyAfternoon =: %s, Hour =: %d, Minute =: %d"), *EarlyAfternoonTime.ToString(), EarlyAfternoonTime.GetHour(), EarlyAfternoonTime.GetMinute());
	UE_LOG(LogDayCycle, Error, TEXT("LateAfternoon =: %s, Hour =: %d, Minute =: %d"), *LateAfternoonTime.ToString(), LateAfternoonTime.GetHour(), LateAfternoonTime.GetMinute());
	UE_LOG(LogDayCycle, Error, TEXT("SunsetTime =: %s, Hour =: %d, Minute =: %d"), *SunsetTime.ToString(), SunsetTime.GetHour(), SunsetTime.GetMinute());
	UE_LOG(LogDayCycle, Error, TEXT("TwilightTime =: %s, Hour =: %d, Minute =: %d"), *TwilightTime.ToString(), TwilightTime.GetHour(), TwilightTime.GetMinute());
	UE_LOG(LogDayCycle, Error, TEXT("EveningTime =: %s, Hour =: %d, Minute =: %d"), *EveningTime.ToString(), EveningTime.GetHour(), EveningTime.GetMinute());
	UE_LOG(LogDayCycle, Error, TEXT("MidnightTime =: %s, Hour =: %d, Minute =: %d"), *MidnightTime.ToString(), MidnightTime.GetHour(), MidnightTime.GetMinute());

	UE_LOG(LogDayCycle, Error, TEXT("CurrentDayPhase =: %s"), *CurrentDayPhase);
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
