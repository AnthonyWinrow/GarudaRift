void ASunPositionController::AdjustSunColor()
{
	// Define start and end times for sunrise, morning, afternoon, evening, and night transitions
	float SunriseStartTime = 6.0f * 60.0f * 60.0f; // 6:00 AM
	float SunriseEndTime = 7.0f * 60.0f * 60.0f; // 7:00 AM
	float MorningEndTime = 12.0f * 60.0f * 60.0f; // 12:00 PM
	float AfternoonEndTime = 17.0f * 60.0f * 60.0f; // 5:00 PM
	float EveningEndTime = 18.0f * 60.0f * 60.0f; // 6:00 PM

	// Calculate the alpha value besed on current time of day
	float Alpha;
	FLinearColor StartColor;
	FLinearColor EndColor;

	if (CurrentTimeOfDay >= SunriseStartTime && CurrentTimeOfDay <= SunriseEndTime)
	{
		// During sunrise, interpolate from night to sunrise color
		Alpha = (CurrentTimeOfDay - SunriseStartTime) / (SunriseEndTime - SunriseStartTime);
		StartColor = FLinearColor::FromSRGBColor(NightColor);
		EndColor = FLinearColor::FromSRGBColor(SunriseColor);
	}
	else if (CurrentTimeOfDay > SunriseEndTime && CurrentTimeOfDay <= MorningEndTime)
	{
		// During morning, interpolate from sunrise to morning color
		Alpha = (CurrentTimeOfDay - SunriseEndTime) / (MorningEndTime - SunriseEndTime);
		StartColor = FLinearColor::FromSRGBColor(NightColor);
		EndColor = FLinearColor::FromSRGBColor(SunriseColor);
	}
	else if (CurrentTimeOfDay > MorningEndTime && CurrentTimeOfDay <= AfternoonEndTime)
	{
		// During afternoon, interpolate from morning to afternoon color
		Alpha = (CurrentTimeOfDay - MorningEndTime) / (AfternoonEndTime - MorningEndTime);
		StartColor = FLinearColor::FromSRGBColor(NightColor);
		EndColor = FLinearColor::FromSRGBColor(SunriseColor);
	}
	else if (CurrentTimeOfDay > AfternoonEndTime && CurrentTimeOfDay <= EveningEndTime)
	{
		// During evening, interpolate from afternoon to evening color
		Alpha = (CurrentTimeOfDay - AfternoonEndTime) / (EveningEndTime - AfternoonEndTime);
		StartColor = FLinearColor::FromSRGBColor(NightColor);
		EndColor = FLinearColor::FromSRGBColor(SunriseColor);
	}
	else
	{
		// During night, use night color
		StartColor = FLinearColor::FromSRGBColor(NightColor);
		EndColor = FLinearColor::FromSRGBColor(NightColor);
		Alpha = 0.0f;
	}

	FLinearColor ResultColor;
	ResultColor.R = FMath::Lerp(StartColor.R, EndColor.R, Alpha);
	ResultColor.G = FMath::Lerp(StartColor.G, EndColor.G, Alpha);
	ResultColor.B = FMath::Lerp(StartColor.G, EndColor.G, Alpha);
	ResultColor.A = FMath::Lerp(StartColor.A, EndColor.A, Alpha);

	SunLight->LightColor = FColor(ResultColor.ToFColor(true));
}
