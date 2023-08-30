#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Components/SplineComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Components/SplineMeshComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "BuildMode.generated.h"

class AOrion;

UCLASS()
class GARUDARIFT_API UBuildMode : public UObject
{
    GENERATED_BODY()

public:
    // Sets default values
    UBuildMode();

    // Input Methods
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void Zoom(float Value);

    // Function Methods
    void LeftClick();
    void UpdateSelectionBox(float DeltaTime, FVector2D InitialClickLocation, FName ClickedControlPointTag);

    // Indicates whether build mode is active
    bool bIsBuildModeActive;

    // Indicates whether the user is currently dragging the mouse
    bool bIsDragging;

    // Activates build mode
    void ActivateBuildMode();

    // Deactivates build mode
    void DeactivateBuildMode();

    // Check if build mode is active
    bool IsBuildModeActive();

    // Camera turn speed
    float TurnSpeed;

    // Zoom Range Variables
    float InitialCameraHeight;
    bool bHasUpdatedInitialCameraHeight;

    // Handles mouse visibility
    void UpdateCursorVisibility();

    // Pointer to Orion
    AOrion* Orion;

    // Pointer to the world (needed to spawn actors)
    UWorld* World;

    // Pointer to SelectionBox
    ASelectionBox* SelectionBox;

private:
    FVector2D PreviousMousePosition;
};
