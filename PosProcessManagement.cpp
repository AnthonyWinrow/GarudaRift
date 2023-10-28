#include "GarudaRift/ActorClasses/PostProcessManagement.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/TextureCube.h"
#include "EngineUtils.h"
#include "TimerManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPostProcessManagement, Log, All);
DEFINE_LOG_CATEGORY(LogPostProcessManagement);

// Sets default values
APostProcessManagement::APostProcessManagement()
	: SceneLighting(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bNewPhaseStart = false;
	LerpAlpha = 0.0f;
	
	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureFinder(TEXT("Texture2D'/Engine/MobileResources/HUD/T_Castle_ThumbstickOutter.T_CastleThumbstickOutter'"));
	if (TextureFinder.Succeeded())
	{
		BokehShapeTexture = TextureFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTextureCube> DawnTexture(TEXT("TextureCube'/Game/GarudaRift/Lighting/HDRI/Dawn.Dawn'"));
	static ConstructorHelpers::FObjectFinder<UTextureCube> SunriseTexture(TEXT("TextureCube'/Game/GarudaRift/Lighting/HDRI/Sunrise.Sunrise'"));
	static ConstructorHelpers::FObjectFinder<UTextureCube> MorningTexture(TEXT("TextureCube'/Game/GarudaRift/Lighting/HDRI/Morning.Morning'"));
	static ConstructorHelpers::FObjectFinder<UTextureCube> LateMorningTexture(TEXT("TextureCube'/Game/GarudaRift/Lighting/HDRI/LateMorning.LateMorning'"));
	static ConstructorHelpers::FObjectFinder<UTextureCube> NoonTexture(TEXT("TextureCube'/Game/GarudaRift/Lighting/HDRI/Noon.Noon'"));
	static ConstructorHelpers::FObjectFinder<UTextureCube> SunsetTexture(TEXT("TextureCube'/Game/GarudaRift/Lighting/HDRI/Sunset.Sunset'"));
	static ConstructorHelpers::FObjectFinder<UTextureCube> TwilightTexture(TEXT("TextureCube'/Game/GarudaRift/Lighting/HDRI/Twilight.Twilight'"));
	static ConstructorHelpers::FObjectFinder<UTextureCube> EveningTexture(TEXT("TextureCube'/Game/GarudaRift/Lighting/HDRI/Evening.Evening'"));
	static ConstructorHelpers::FObjectFinder<UTextureCube> NightTexture(TEXT("TextureCube'/Game/GarudaRift/Lighting/HDRI/Night.Night'"));

	if (DawnTexture.Succeeded() && SunriseTexture.Succeeded() && MorningTexture.Succeeded() && LateMorningTexture.Succeeded() && NoonTexture.Succeeded() && SunsetTexture.Succeeded() && TwilightTexture.Succeeded() && EveningTexture.Succeeded() && NightTexture.Succeeded())
	{
		DayPhaseTextures.Add(DawnTexture.Object);
		DayPhaseTextures.Add(SunriseTexture.Object);
		DayPhaseTextures.Add(MorningTexture.Object);
		DayPhaseTextures.Add(LateMorningTexture.Object);
		DayPhaseTextures.Add(NoonTexture.Object);
		DayPhaseTextures.Add(SunsetTexture.Object);
		DayPhaseTextures.Add(TwilightTexture.Object);
		DayPhaseTextures.Add(EveningTexture.Object);
		DayPhaseTextures.Add(NightTexture.Object);
	}
	else
	{
		UE_LOG(LogPostProcessManagement, Error, TEXT("Failed to load one or more HDRI textures"));
	}
}

// Called when the game starts or when spawned
void APostProcessManagement::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<APostProcessVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->ActorHasTag(FName("SceneLighting")))
		{
			SceneLighting = *ActorItr;
		}

		UE_LOG(LogPostProcessManagement, Log, TEXT("Found Post Process Volume =: %s"), *ActorItr->GetName());
	}

	if (SceneLighting)
	{
		SceneLighting->bEnabled = true;
		SceneLighting->BlendWeight = 1.0f;

		FPostProcessSettings& PostProcessSettings = SceneLighting->Settings;

		// Bloom Initializations
		PostProcessSettings.BloomMethod = BM_SOG;
		PostProcessSettings.BloomIntensity = 0.0f;
		PostProcessSettings.BloomThreshold = 0.0f;
		PostProcessSettings.BloomConvolutionScatterDispersion = -1.0f;

		// Auto Expsosure Initializationss
		PostProcessSettings.AutoExposureMethod = AEM_Histogram;
		PostProcessSettings.AutoExposureBias = 1.0f;
		PostProcessSettings.AutoExposureMinBrightness = -10.0f;
		PostProcessSettings.AutoExposureMaxBrightness = 20.0f;
		PostProcessSettings.AutoExposureSpeedUp = 3.0f;
		PostProcessSettings.AutoExposureSpeedDown = 1.0f;

		// Lense Flare Initializations
		PostProcessSettings.LensFlareIntensity = 3.816f;
		PostProcessSettings.LensFlareTint = FLinearColor::FromSRGBColor(FColor::FromHex("FFFFFF"));
		PostProcessSettings.LensFlareBokehSize = 6.656f;
		PostProcessSettings.LensFlareThreshold = 8.0f;
		if (BokehShapeTexture)
		{
			PostProcessSettings.LensFlareBokehShape = BokehShapeTexture;
		}
		else
		{
			UE_LOG(LogPostProcessManagement, Error, TEXT("BokehShapeTexture Not Found"));
		}

		// Vignette Initialization
		PostProcessSettings.VignetteIntensity = 0.0f;
		
		// Temperature Initialization
		PostProcessSettings.TemperatureType = TEMP_ColorTemperature;
		PostProcessSettings.WhiteTemp = 6500.0f;

		// Global Color Grading Initialization
		PostProcessSettings.ColorSaturation = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorContrast = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorGamma = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorGain = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorOffset = FVector4(0.0f, 0.0f, 0.0f, 0.0f);

		// Shadow Grading Initialization
		PostProcessSettings.ColorSaturationShadows = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorContrastShadows = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorGammaShadows = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorGainShadows = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorOffsetShadows = FVector4(0.0f, 0.0f, 0.0f, 0.0f);
		PostProcessSettings.ColorCorrectionShadowsMax = 0.09f;

		// Midtone Grading Initialization
		PostProcessSettings.ColorSaturationMidtones = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorContrastMidtones = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorGammaMidtones = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorGainMidtones = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorOffsetMidtones = FVector4(0.0f, 0.0f, 0.0f, 0.0f);

		// Highlight Grading Initialization
		PostProcessSettings.ColorSaturationHighlights = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorContrastHighlights = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorGammaHighlights = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		PostProcessSettings.ColorGainHighlights = FVector4(1.0f, 1.0, 1.0f, 1.0f);
		PostProcessSettings.ColorOffsetHighlights = FVector4(0.0f, 0.0f, 0.0f, 0.0f);
		PostProcessSettings.ColorCorrectionHighlightsMin = 0.0f;
		PostProcessSettings.ColorCorrectionHighlightsMax = 0.0f;

		// Initializing Film Settings
		PostProcessSettings.FilmSlope = 0.0f;
		PostProcessSettings.FilmToe = 0.0f;
		PostProcessSettings.FilmShoulder = 0.0f;
		PostProcessSettings.FilmBlackClip = 0.0f;
		PostProcessSettings.FilmWhiteClip = 0.0f;

		// Ambient Cubemap Initialization
		PostProcessSettings.AmbientCubemapIntensity = 0.0f;
		PostProcessSettings.AmbientCubemapTint = FLinearColor::White;
		PostProcessSettings.AmbientCubemap = nullptr;
	}
}

// Called every frame
void APostProcessManagement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ShiftSceneLightingDynamics(DeltaTime);
}

void APostProcessManagement::UpdatePostProcess(const FString& CurrentDayPhase)
{
	if (CurrentDayPhaseState != CurrentDayPhase)
	{
		CurrentDayPhaseState = CurrentDayPhase;

		UE_LOG(LogPostProcessManagement, Log, TEXT("CurrentDayPhaseState Updated =: %s"), *CurrentDayPhaseState);
	}
}

void APostProcessManagement::ShiftSceneLightingDynamics(float DeltaTime)
{
	FPostProcessSettings& PostProcessSettings = SceneLighting->Settings;

	float Speed = 0.04f;
	float DawnBloomIntensity = 0.832f;
	float SunriseBloomIntensity = 2.339f;
	float DawnAutoExposureBias = 1.0f;
	float SunriseAutoExposureBias = -0.899999;
	float DawnAutoExposureMinBrightness = -1.0f;
	float SunriseAutoExposureMinBrightness = 1.0f;
	float DawnAutoExposureMaxBrightness = 1.5f;
	float SunriseAutoExposureMaxBrightness = 9.0f;
	float DawnWhiteTemp = 4740.0f;
	float SunriseWhiteTemp = 7796.0f;
	FVector4 DawnColorSaturation(0.8f, 0.8f, 0.8f, 1.0f);
	FVector4 SunriseColorSaturation(0.98f, 0.98f, 0.98f, 1.0f);
	FVector4 DawnColorContrast(1.06f, 1.06f, 1.06f, 1.0f);
	FVector4 SunriseColorContrast(1.2f, 1.2f, 1.2f, 1.0f);
	FVector4 DawnColorGamma(1.05f, 1.05f, 1.05f, 1.0f);
	FVector4 SunriseColorGamma(0.94f, 0.94f, 0.94, 1.0f);
	FVector4 DawnColorGain(0.9f, 0.9f, 0.9f, 1.0f);
	FVector4 SunriseColorGain(1.18f, 1.18f, 1.18f, 1.0f);
	FVector4 DawnColorOffset(-0.02f, -0.02f, -0.02f, 0.0f);
	FVector4 SunriseColorOffset(0.01f, 0.01f, 0.01f, 0.0f);

	LerpAlpha += DeltaTime * Speed;
	LerpAlpha = FMath::Clamp(LerpAlpha, 0.0f, 1.0f);

	if (CurrentDayPhaseState == "Dawn")
	{
		PostProcessSettings.BloomIntensity = FMath::Lerp(DawnBloomIntensity, SunriseBloomIntensity, LerpAlpha);
		PostProcessSettings.AutoExposureBias = FMath::Lerp(DawnAutoExposureBias, SunriseAutoExposureBias, LerpAlpha);
		PostProcessSettings.AutoExposureMinBrightness = FMath::Lerp(DawnAutoExposureMinBrightness, SunriseAutoExposureMinBrightness, LerpAlpha);
		PostProcessSettings.AutoExposureMaxBrightness = FMath::Lerp(DawnAutoExposureMaxBrightness, SunriseAutoExposureMaxBrightness, LerpAlpha);
		PostProcessSettings.WhiteTemp = FMath::Lerp(DawnWhiteTemp, SunriseWhiteTemp, LerpAlpha);
		PostProcessSettings.ColorSaturation = FMath::Lerp(DawnColorSaturation, SunriseColorSaturation, LerpAlpha);
		PostProcessSettings.ColorContrast = FMath::Lerp(DawnColorContrast, SunriseColorContrast, LerpAlpha);
		PostProcessSettings.ColorGamma = FMath::Lerp(DawnColorGamma, SunriseColorGamma, LerpAlpha);
		PostProcessSettings.ColorGain = FMath::Lerp(DawnColorGain, SunriseColorGain, LerpAlpha);
		PostProcessSettings.ColorOffset = FMath::Lerp(DawnColorOffset, SunriseColorOffset, LerpAlpha);

		if (SceneLighting)
		{
			PostProcessSettings.BloomThreshold = -1.0f;
			PostProcessSettings.ColorSaturationShadows = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
			PostProcessSettings.ColorContrastShadows = FVector4(1.1f, 1.1f, 1.1f, 1.1f);
			PostProcessSettings.ColorGammaShadows = FVector4(1.22f, 1.22f, 1.22f, 1.22f);
			PostProcessSettings.ColorGainShadows = FVector4(1.02f, 1.02f, 1.02f, 1.0f);
			PostProcessSettings.ColorOffsetShadows = FVector4(0.0f, 0.0f, 0.0f, 0.0f);
			PostProcessSettings.ColorCorrectionShadowsMax = 0.09;
			PostProcessSettings.ColorSaturationMidtones = FVector4(0.9f, 0.9f, 0.9f, 1.0f);
			PostProcessSettings.ColorContrastMidtones = FVector4(0.92f, 0.92f, 0.92f, 1.0f);
			PostProcessSettings.ColorGammaMidtones = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
			PostProcessSettings.ColorGainMidtones = FVector4(0.88f, 0.88f, 0.88f, 1.0f);
			PostProcessSettings.ColorOffsetMidtones = FVector4(0.0f, 0.0f, 0.0f, 0.0f);
			PostProcessSettings.ColorSaturationHighlights = FVector4(1.08f, 1.08f, 1.08f, 1.0f);
			PostProcessSettings.ColorContrastHighlights = FVector4(1.02f, 1.02f, 1.02f, 1.0f);
			PostProcessSettings.ColorGammaHighlights = FVector4(0.96f, 0.96f, 0.96f, 1.0f);
			PostProcessSettings.ColorGainHighlights = FVector4(1.08f, 1.08f, 1.08f, 1.0f);
			PostProcessSettings.ColorGainHighlights = FVector4(-0.04f, -0.04f, -0.04f, 0.0f);
			PostProcessSettings.ColorCorrectionHighlightsMax = -0.072;
			PostProcessSettings.ColorCorrectionHighlightsMin = 1.432;
			PostProcessSettings.FilmSlope = 0.92;
			PostProcessSettings.FilmToe = 0.6;
			PostProcessSettings.FilmShoulder = 0.28;
			PostProcessSettings.FilmWhiteClip = 0.06;
			PostProcessSettings.AmbientCubemap = DayPhaseTextures[0];
			PostProcessSettings.AmbientCubemapIntensity = 0.1f;
			PostProcessSettings.AmbientCubemapTint = FLinearColor::FromSRGBColor(FColor::FromHex("B3D7FFFF"));			
		}
		UE_LOG(LogPostProcessManagement, Log, TEXT("Current Phase: Dawn, Bloom Intensity =: %f, LerpAlpha =: %f"), PostProcessSettings.BloomIntensity, LerpAlpha);
		UE_LOG(LogPostProcessManagement, Log, TEXT("Ambient Cubemap Set to Dawn"));
	}
	else if (CurrentDayPhaseState == "Sunrise")
	{
		LerpAlpha = 0.0f;
		if (SceneLighting)
		{
			PostProcessSettings.BloomIntensity = 2.339f;
			PostProcessSettings.BloomThreshold = -1.0f;
			PostProcessSettings.BloomConvolutionScatterDispersion = 2.24;
			PostProcessSettings.AutoExposureBias = -0.899999;
			PostProcessSettings.AutoExposureApplyPhysicalCameraExposure = 1.0;
			PostProcessSettings.AutoExposureMinBrightness = 1.0f;
			PostProcessSettings.AutoExposureMaxBrightness = 9.0f;
			PostProcessSettings.AutoExposureSpeedUp = 3.0f;
			PostProcessSettings.AutoExposureSpeedDown = 1.0f;
			PostProcessSettings.WhiteTemp = 7796.0;
			PostProcessSettings.ColorSaturation = FVector4(0.98f, 0.98f, 0.98f, 1.0f);
			PostProcessSettings.ColorContrast = FVector4(1.2f, 1.2f, 1.2f, 1.0f);
			PostProcessSettings.ColorGamma = FVector4(0.94f, 0.94f, 0.94f, 1.0f);
			PostProcessSettings.ColorGain = FVector4(1.18f, 1.18f, 1.18f, 1.0f);
			PostProcessSettings.ColorOffset = FVector4(0.01f, 0.01f, 0.01f, 0.0f);
			PostProcessSettings.ColorSaturationShadows = FVector4(1.04f, 1.04f, 1.04f, 1.0f);
			PostProcessSettings.ColorContrastShadows = FVector4(0.9f, 0.9f, 0.9f, 1.0f);
			PostProcessSettings.ColorGammaShadows = FVector4(1.1f, 1.1f, 1.1f, 1.0f);
			PostProcessSettings.ColorGainShadows = FVector4(1.14f, 1.14f, 1.14f, 1.0f);
			PostProcessSettings.ColorOffsetShadows = FVector4(-0.04f, -0.04f, -0.04f, 0.0f);
			PostProcessSettings.ColorCorrectionShadowsMax = 0.09;
			PostProcessSettings.ColorSaturationMidtones = FVector4(1.08f, 1.08f, 1.08f, 1.0);
			PostProcessSettings.ColorContrastMidtones = FVector4(1.06f, 1.06f, 1.06f, 1.0f);
			PostProcessSettings.ColorGammaMidtones = FVector4(1.12f, 1.12f, 1.12f, 1.0f);
			PostProcessSettings.ColorGainMidtones = FVector4(0.96f, 0.96f, 0.96f, 1.0f);
			PostProcessSettings.ColorOffsetMidtones = FVector4(0.02f, 0.02f, 0.02f, 0.0f);
			PostProcessSettings.ColorSaturationHighlights = FVector4(0.78f, 0.78f, 0.78f, 1.0f);
			PostProcessSettings.ColorContrastHighlights = FVector4(1.14f, 1.14f, 1.14f, 1.0f);
			PostProcessSettings.ColorGammaHighlights = FVector4(0.98f, 0.98f, 0.98f, 1.0f);
			PostProcessSettings.ColorGainHighlights = FVector4(0.7f, 0.7f, 0.7f, 1.0f);
			PostProcessSettings.ColorOffsetHighlights = FVector4(0.08f, 0.08f, 0.08f, 0.0f);
			PostProcessSettings.AmbientCubemap = DayPhaseTextures[1];
			PostProcessSettings.AmbientCubemapIntensity = 0.4f;
			PostProcessSettings.AmbientCubemapTint = FLinearColor::White;			
		}
		UE_LOG(LogPostProcessManagement, Log, TEXT("Current Phase: Sunrise, Bloom Intensity =: %f"), PostProcessSettings.BloomIntensity);
	}
}
