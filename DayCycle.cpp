#include "GarudaRift/ActorClasses/DayCycle.h"
#include "GarudaRift/ActorClasses/PostProcessManagement.h"
#include "GarudaRift/SeasonEnum.h"
#include "SunPosition.h"
#include "EngineUtils.h"
#include "Components/SkyLightComponent.h"

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

	Skylight = CreateDefaultSubobject<USkyLightComponent>(TEXT("Skylight"));
	Skylight->SetupAttachment(Root);

	TimeScale = 1.0f;
	Latitude = 0.0f;
	Longitude = 0.0f;
	Timezone = 0.0f;
	bDaylightSavings = false;
	CurrentPhaseIndex = 0;
	CurrentIntensityIndex = 0;
	CurrentSkylightIntensityIndex = 0;
	NextPhaseIndex = 1;
	NextIntensityIndex = 1;
	NextSkylightIntensityIndex = 1;
	NormalizedTime = 0.0f;
	NormalizedTimeIntensity = 0.0f;
	bInterpolate = true;
	bInterpolateIntensity = true;
	Year = 2023;
	Month = 10;
	Day = 8;
	Hours = 13;
	Minutes = 0;
	Seconds = 0;
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

	if (PhaseColors.Num() == 0)
	{
		// Color Array
		PhaseColors = {
			FLinearColor::FromSRGBColor(FColor::FromHex("FFA040")), // Dawn
			FLinearColor::FromSRGBColor(FColor::FromHex("FFE8B4")), // Sunrise
			FLinearColor::FromSRGBColor(FColor::FromHex("FFDA5EFF")), // Morning
			FLinearColor::FromSRGBColor(FColor::FromHex("FFF5AA")), // Late Morning
			FLinearColor::FromSRGBColor(FColor::FromHex("FFFFFF")), // Noon
			FLinearColor::FromSRGBColor(FColor::FromHex("FFEBD4")), // Early Afternoon
			FLinearColor::FromSRGBColor(FColor::FromHex("FEF9E7")), // Late Afternoon
			FLinearColor::FromSRGBColor(FColor::FromHex("FF8C00")), // Sunset
			FLinearColor::FromSRGBColor(FColor::FromHex("FF6B3F")), // Twilight
			FLinearColor::FromSRGBColor(FColor::FromHex("FF8C42")), // Evening
			FLinearColor::FromSRGBColor(FColor::FromHex("463D2C")), // Night
		};
	}

	if (BloomTints.Num() == 0)
	{
		BloomTints = {
			FLinearColor::FromSRGBColor(FColor::FromHex("FFA500FF"))
		};
	}

	if (BloomScales.Num() == 0)
	{
		BloomScales = {
			0.5f, 0.01f, 0.01f, 0.01f, 0.01f,
			0.01f, 0.01f, 0.01f, 0.01f, 0.01f,
		};
	}

	Sunlight->BloomScale = BloomScales[0];

	if (PhaseIntensities.Num() == 0)
	{
		PhaseIntensities = {
			10.0f, 400.0f, 2000.0f, 4000.0f, 120000.0f,
			6000.0f, 3000.0f, 400.0f, 20.0f, 15.0f,
			10.0f, 5.0f, 10.0f
		};

		if (PhaseColors.Num() > 0)
		{
			CurrentPhaseIndex = 0;
			NextPhaseIndex = (CurrentPhaseIndex + 1) % PhaseColors.Num();
		}
	}

	if (PhaseIntensities.Num() > 0)
	{
		CurrentIntensityIndex = 0;
		NextIntensityIndex = (CurrentIntensityIndex + 1) % PhaseIntensities.Num();
	}

	Sunlight->SetLightColor(FColor::Black);
	Sunlight->Intensity = PhaseIntensities[CurrentIntensityIndex];

	if (SkylightPhaseIntensities.Num() == 0)
	{
		SkylightPhaseIntensities = {
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f
		};

		CurrentSkylightIntensityIndex = 0;
		NextSkylightIntensityIndex = (CurrentSkylightIntensityIndex + 1) % SkylightPhaseIntensities.Num();
	}

	Skylight->Intensity = SkylightPhaseIntensities[CurrentSkylightIntensityIndex];

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

	CurrentTime = FDateTime(Year, Month, Day, Hours, Minutes, static_cast<int32>(Seconds));

	UpdateSeason();
	if (CurrentSeason == ESeason::Spring)
	{
		UpdateSpring();
	}
	else if (CurrentSeason == ESeason::Summer)
	{
		UpdateSummer();
	}
	else if (CurrentSeason == ESeason::Autumn)
	{
		UpdateAutumn();
	}
	else if (CurrentSeason == ESeason::Winter)
	{
		UpdateWinter();
	}

	UpdateDayPhase();
	SunlightIntensity(DeltaTime);
	SunLightColor(DeltaTime);
	SkylightIntensity(DeltaTime);
	SunlightBloomScale(DeltaTime);
	UpdateSunPosition();
}

void ADayCycle::SunlightIntensity(float DeltaTime)
{
	float TargetIntensity;
	float LerpSpeed = 0.25f;

	if (LastIntensityPhase != CurrentDayPhase)
	{
		LastIntensityPhase = CurrentDayPhase;
		CurrentIntensityIndex = (CurrentIntensityIndex + 1) % PhaseIntensities.Num();
		NextIntensityIndex = (CurrentIntensityIndex + 1) % PhaseIntensities.Num();
		NormalizedTimeIntensity = 0.0f;
		bInterpolateIntensity = true;
	}

	if (bInterpolateIntensity)
	{
		NormalizedTimeIntensity += DeltaTime * LerpSpeed;
	}

	// Phase Check
	if (NormalizedTimeIntensity >= 1.0f)
	{
		NormalizedTimeIntensity = 0.0f;
		bInterpolateIntensity = false;
	}
	
	if (bInterpolateIntensity)
	{
		TargetIntensity = FMath::Lerp(PhaseIntensities[CurrentIntensityIndex], PhaseIntensities[NextIntensityIndex], NormalizedTimeIntensity);
		Sunlight->Intensity = FMath::Lerp(Sunlight->Intensity, TargetIntensity, DeltaTime * LerpSpeed);
	}

	Sunlight->MarkRenderStateDirty();
}

void ADayCycle::SunLightColor(float DeltaTime)
{
	float LerpSpeed = 0.5f;
	static bool bColorCycle = true;

	if (CurrentPhaseIndex == 0)
	{
		bColorCycle = true;
	}

	if (LastDayPhase != CurrentDayPhase && bColorCycle)
	{
		LastDayPhase = CurrentDayPhase;
		CurrentPhaseIndex = (CurrentPhaseIndex + 1) % PhaseColors.Num();

		if (CurrentPhaseIndex == PhaseColors.Num() - 1)
		{
			NextPhaseIndex = 0;
			bColorCycle = false;
		}
		else
		{
			NextPhaseIndex = (CurrentPhaseIndex + 1) % PhaseColors.Num();
		}

		NormalizedTime = 0.0f;
		bInterpolate = true;
	}

	if (bInterpolate)
	{
		NormalizedTime += DeltaTime * LerpSpeed;
	}

	// Phase Check
	if (NormalizedTime > 1.0f)
	{
		NormalizedTime = 0.0f;
		bInterpolate = false;
	}
	else
	{
		bInterpolate = true;
	}

	FLinearColor TargetColor = FLinearColor::LerpUsingHSV(PhaseColors[CurrentPhaseIndex], PhaseColors[NextPhaseIndex], NormalizedTime);
	FLinearColor CurrentLinearColor = FLinearColor(Sunlight->LightColor);
	FLinearColor InterpolatedColor = FLinearColor::LerpUsingHSV(CurrentLinearColor, TargetColor, DeltaTime * LerpSpeed);

	FColor NewColor = InterpolatedColor.ToFColor(true);
	Sunlight->LightColor = NewColor;
	Sunlight->MarkRenderStateDirty();
}

void ADayCycle::SkylightIntensity(float DeltaTime)
{
	float TargetIntensity;
	float LerpSpeed = 0.25f;

	if (LastSkylightIntensityPhase != CurrentDayPhase)
	{
		LastSkylightIntensityPhase = CurrentDayPhase;
		CurrentSkylightIntensityIndex = (CurrentSkylightIntensityIndex + 1) % SkylightPhaseIntensities.Num();
		NextSkylightIntensityIndex = (CurrentSkylightIntensityIndex + 1) % SkylightPhaseIntensities.Num();
		NormalizedTimeIntensity = 0.0f;
		bInterpolateIntensity = true;
	}

	if (bInterpolateIntensity)
	{
		NormalizedTimeIntensity += DeltaTime * LerpSpeed;
	}

	if (NormalizedTimeIntensity >= 1.0f)
	{
		NormalizedTimeIntensity = 0.0f;
		bInterpolateIntensity = false;
	}

	if (bInterpolateIntensity)
	{
		TargetIntensity = FMath::Lerp(SkylightPhaseIntensities[CurrentSkylightIntensityIndex], SkylightPhaseIntensities[NextSkylightIntensityIndex], NormalizedTimeIntensity);
		Skylight->Intensity = FMath::Lerp(Skylight->Intensity, TargetIntensity, DeltaTime * LerpSpeed);
	}

	Skylight->MarkRenderStateDirty();
}

void ADayCycle::SunlightBloomScale(float DeltaTime)
{
	float TargetBloomScale;
	float LerpSpeed = 0.25f;

	if (LastBloomPhase != CurrentDayPhase)
	{
		LastBloomPhase = CurrentDayPhase;
		CurrentBloomScaleIndex = (CurrentBloomScaleIndex + 1) % BloomScales.Num();
		NextBloomScaleIndex = (CurrentBloomScaleIndex + 1) % BloomScales.Num();
		NormalizedTimeBloomScale = 0.0f;
		bInterpolateBloomScale = true;
	}

	if (bInterpolateBloomScale)
	{
		NormalizedTimeBloomScale += DeltaTime * LerpSpeed;
	}

	if (NormalizedTimeBloomScale >= 1.0)
	{
		NormalizedTimeBloomScale = 0.0f;
		bInterpolateBloomScale = false;
	}

	if (bInterpolateBloomScale)
	{
		TargetBloomScale = FMath::Lerp(BloomScales[CurrentBloomScaleIndex], BloomScales[NextBloomScaleIndex], NormalizedTimeBloomScale);
		Sunlight->BloomScale = FMath::Lerp(Sunlight->BloomScale, TargetBloomScale, DeltaTime * LerpSpeed);
	}

	Sunlight->MarkRenderStateDirty();
}

void ADayCycle::SunlightBloomTint(float DeltaTime)
{
	float LerpSpeed = 0.5f;
	static bool bColorCycle = true;

	if (CurrentBloomTintIndex == 0)
	{
		bColorCycle = true;
	}

	if (LastBloomTintPhase != CurrentDayPhase && bColorCycle)
	{
		LastBloomTintPhase = CurrentDayPhase;
		CurrentBloomTintIndex = (CurrentBloomTintIndex + 1) % BloomTints.Num();

		if (CurrentBloomTintIndex == BloomTints.Num() - 1)
		{
			NextBloomTintIndex = 0;
			bColorCycle = false;
		}
		else
		{
			NextBloomTintIndex = (CurrentBloomTintIndex + 1) % BloomTints.Num();
		}

		NormalizedTimeBloomTint = 0.0f;
		bInterpolateBloomTint = true;
	}

	if (bInterpolateBloomTint)
	{
		NormalizedTimeBloomTint += DeltaTime * LerpSpeed;
	}

	if (NormalizedTimeBloomTint > 1.0f)
	{
		NormalizedTimeBloomTint = 0.0f;
		bInterpolateBloomTint = false;
	}

	FLinearColor TargetBloomTint = FLinearColor::LerpUsingHSV(BloomTints[CurrentBloomTintIndex], BloomTints[NextBloomTintIndex], NormalizedTimeBloomTint);
	FLinearColor CurrentBloomTint = Sunlight->BloomTint;
	FLinearColor InterpolatedBloomTint = FLinearColor::LerpUsingHSV(CurrentBloomTint, TargetBloomTint, DeltaTime * LerpSpeed);

	FColor NewBloomTint = InterpolatedBloomTint;
	Sunlight->BloomTint = InterpolatedBloomTint;
	Sunlight->MarkRenderStateDirty();
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
		CurrentSeason = ESeason::Spring;
	}
	else if (Month >= 6 && Month <= 8)
	{
		CurrentSeason = ESeason::Summer;
	}
	else if (Month >= 9 && Month <= 10)
	{
		CurrentSeason = ESeason::Autumn;
	}
	else if (Month >= 11 && Month <= 3)
	{
		CurrentSeason = ESeason::Winter;
	}
}

void ADayCycle::UpdateSpring()
{
	DawnTime = FDateTime(Year, Month, Day, 8, 0, 0);
	SunriseTime = FDateTime(Year, Month, Day, 9, 0, 0);
	MorningTime = FDateTime(Year, Month, Day, 10, 0, 0);
	LateMorningTime = FDateTime(Year, Month, Day, 11, 0, 0);
	NoonTime = FDateTime(Year, Month, Day, 12, 0, 0);
	EarlyAfternoonTime = FDateTime(Year, Month, Day, 14, 0, 0);
	LateAfternoonTime = FDateTime(Year, Month, Day, 16, 0, 0);
	SunsetTime = FDateTime(Year, Month, Day, 18, 0, 0);
	TwilightTime = FDateTime(Year, Month, Day, 20, 0, 0);
	EveningTime = FDateTime(Year, Month, Day, 22, 0, 0);
	NightTime = FDateTime(Year, Month, Day, 23, 0, 0);
	MidnightTime = FDateTime(Year, Month, Day, 1, 0, 0);
	LateNightTime = FDateTime(Year, Month, Day, 3, 0, 0);
}

void ADayCycle::UpdateSummer()
{
	DawnTime = FDateTime(Year, Month, Day, 6, 0, 0);
	SunriseTime = FDateTime(Year, Month, Day, 7, 0, 0);
	MorningTime = FDateTime(Year, Month, Day, 8, 0, 0);
	LateMorningTime = FDateTime(Year, Month, Day, 9, 0, 0);
	NoonTime = FDateTime(Year, Month, Day, 12, 0, 0);
	EarlyAfternoonTime = FDateTime(Year, Month, Day, 14, 0, 0);
	LateAfternoonTime = FDateTime(Year, Month, Day, 16, 0, 0);
	SunsetTime = FDateTime(Year, Month, Day, 20, 0, 0);
	TwilightTime = FDateTime(Year, Month, Day, 21, 0, 0);
	EveningTime = FDateTime(Year, Month, Day, 22, 0, 0);
	NightTime = FDateTime(Year, Month, Day, 23, 0, 0);
	MidnightTime = FDateTime(Year, Month, Day, 1, 0, 0);
	LateNightTime = FDateTime(Year, Month, Day, 2, 0, 0);
}

void ADayCycle::UpdateAutumn()
{
	DawnTime = FDateTime(Year, Month, Day, 7, 0, 0);
	SunriseTime = FDateTime(Year, Month, Day, 9, 0, 0);
	MorningTime = FDateTime(Year, Month, Day, 10, 0, 0);
	LateMorningTime = FDateTime(Year, Month, Day, 11, 0, 0);
	NoonTime = FDateTime(Year, Month, Day, 12, 0, 0);
	EarlyAfternoonTime = FDateTime(Year, Month, Day, 14, 0, 0);
	LateAfternoonTime = FDateTime(Year, Month, Day, 16, 0, 0);
	SunsetTime = FDateTime(Year, Month, Day, 18, 0, 0);
	TwilightTime = FDateTime(Year, Month, Day, 19, 0, 0);
	EveningTime = FDateTime(Year, Month, Day, 20, 0, 0);
	NightTime = FDateTime(Year, Month, Day, 22, 0, 0);
	MidnightTime = FDateTime(Year, Month, Day, 23, 0, 0);
	LateNightTime = FDateTime(Year, Month, Day, 1, 0, 0);
}

void ADayCycle::UpdateWinter()
{
	DawnTime = FDateTime(Year, Month, Day, 8, 0, 0);
	SunriseTime = FDateTime(Year, Month, Day, 9, 0, 0);
	MorningTime = FDateTime(Year, Month, Day, 10, 0, 0);
	LateMorningTime = FDateTime(Year, Month, Day, 11, 0, 0);
	NoonTime = FDateTime(Year, Month, Day, 12, 0, 0);
	EarlyAfternoonTime = FDateTime(Year, Month, Day, 13, 0, 0);
	LateAfternoonTime = FDateTime(Year, Month, Day, 15, 0, 0);
	SunsetTime = FDateTime(Year, Month, Day, 16, 0, 0);
	TwilightTime = FDateTime(Year, Month, Day, 17, 0, 0);
	EveningTime = FDateTime(Year, Month, Day, 18, 0, 0);
	NightTime = FDateTime(Year, Month, Day, 19, 0, 0);
	MidnightTime = FDateTime(Year, Month, Day, 21, 0, 0);
	LateNightTime = FDateTime(Year, Month, Day, 23, 0, 0);
}

void ADayCycle::UpdateDayPhase()
{
	int32 CurrentHour = CurrentTime.GetHour();
	int32 CurrentMinute = CurrentTime.GetMinute();

	int32 CurrentSecond = CurrentTime.GetSecond();

	if (CurrentHour >= DawnTime.GetHour() && CurrentHour < SunriseTime.GetHour())
	{
		CurrentDayPhase = Dawn;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= SunriseTime.GetHour() && CurrentHour < MorningTime.GetHour())
	{
		CurrentDayPhase = Sunrise;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= MorningTime.GetHour() && CurrentHour < LateMorningTime.GetHour())
	{
		CurrentDayPhase = Morning;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= LateMorningTime.GetHour() && CurrentHour < NoonTime.GetHour())
	{
		CurrentDayPhase = LateMorning;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= NoonTime.GetHour() && CurrentHour < EarlyAfternoonTime.GetHour())
	{
		CurrentDayPhase = Noon;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= EarlyAfternoonTime.GetHour() && CurrentHour < LateAfternoonTime.GetHour())
	{
		CurrentDayPhase = EarlyAfternoon;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= LateAfternoonTime.GetHour() && CurrentHour < SunsetTime.GetHour())
	{
		CurrentDayPhase = LateAfternoon;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= SunsetTime.GetHour() && CurrentHour < TwilightTime.GetHour())
	{
		CurrentDayPhase = Sunset;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= TwilightTime.GetHour() && CurrentHour < EveningTime.GetHour())
	{
		CurrentDayPhase = Twilight;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= EveningTime.GetHour() && CurrentHour < NightTime.GetHour())
	{
		CurrentDayPhase = Evening;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= NightTime.GetHour() && CurrentHour < MidnightTime.GetHour())
	{
		CurrentDayPhase = Night;
		UpdatePostProcessManagement();
	}
	else if (CurrentHour >= MidnightTime.GetHour() || (CurrentHour >= 0 && CurrentHour < LateNightTime.GetHour()))
	{
		CurrentDayPhase = Midnight;
		UpdatePostProcessManagement();
	}
	else
	{
		CurrentDayPhase = LateNight;
		UpdatePostProcessManagement();
	}
}

void ADayCycle::UpdatePostProcessManagement()
{
	for (TActorIterator<APostProcessManagement> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		APostProcessManagement* PostProcessManager = *ActorItr;
		if (PostProcessManager)
		{
			PostProcessManager->UpdatePostProcess(CurrentDayPhase);
			break;
		}
		else
		{
			UE_LOG(LogDayCycle, Error, TEXT("PostProcessManager is NULL"));
		}
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
