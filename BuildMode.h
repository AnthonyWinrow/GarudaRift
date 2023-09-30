#pragma once

#include "CoreMinimal.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Components/SplineComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Components/SplineMeshComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "BuildMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBuildMode_Cameras, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogBuildMode_Movement, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogBuildMode_Inputs, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogBuildMode_Destruction, Log, All);

class AOrion;

UCLASS()
class GARUDARIFT_API UBuildMode : public UObject
{
    GENERATED_BODY()

public:
    // Sets default values
    UBuildMode();

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void Zoom(float Value);
    void LeftClick(bool bIsPressed);
    void RightClick(bool bRightPressed);
    void UpdateCursorVisibility();
    void ActivateBuildMode();
    void DeactivateBuildMode();

    bool bIsLeftMouseCurrentlyPressed;
    bool bIsRightMouseCurrentlyPressed;
    bool bIsSelectionBoxSpawned = false;
    bool bRightMousePressed;
    bool bIsBuildModeActive;
    bool bIsDragging;
    bool bHasUpdatedInitialCameraHeight;
    bool IsBuildModeActive();
    float TurnSpeed;
    float InitialCameraHeight;
    
    AOrion* Orion;
    UWorld* World;
    ASelectionBox* SelectionBox;

private:
    FVector2D PreviousMousePosition;
};
