#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FreeLookMode.generated.h"

UCLASS()
class GARUDARIFT_API UFreeLookMode : public UObject
{
	GENERATED_BODY()

public:
	UFreeLookMode();

	// Handles Turning
	void Turn(float Value);

	void ActivateFreeLookMode();
	void DeactivateFreeLookMode();
	bool IsFreeLookModeActive();

	bool bIsFreeLookModeActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AOrion")
	class AOrion* Orion;
};
