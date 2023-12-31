#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/CharacterLogic/BuildMode.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/Characters/Orion.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Engine/World.h"

DEFINE_LOG_CATEGORY(LogBuildMode_Cameras);
DEFINE_LOG_CATEGORY(LogBuildMode_Movement);
DEFINE_LOG_CATEGORY(LogBuildMode_Inputs);
DEFINE_LOG_CATEGORY(LogBuildMode_Destruction);

// Sets default values
UBuildMode::UBuildMode()
{
    bIsBuildModeActive = false;
    TurnSpeed = 20.0f;
    bHasUpdatedInitialCameraHeight = false;
    bIsDragging = false;
}

void UBuildMode::UpdateCursorVisibility()
{
    if (Orion && Orion->bRightMousePressed)
    {
        // Hide mouse cursor
        APlayerController* PlayerController = Cast<APlayerController>(Orion->GetController());
        if (PlayerController)
        {
            PlayerController->bShowMouseCursor = false;
            PlayerController->bEnableClickEvents = false;
            PlayerController->bEnableMouseOverEvents = false;
        }
    }
    else
    {
        // Show mouse cursor
        APlayerController* PlayerController = Cast<APlayerController>(Orion->GetController());
        if (PlayerController)
        {
            PlayerController->bShowMouseCursor = true;
            PlayerController->bEnableClickEvents = true;
            PlayerController->bEnableMouseOverEvents = true;
        }
    }
}

void UBuildMode::MoveForward(float Value)
{
    if (bIsBuildModeActive && Orion && Orion->bRightMousePressed && Orion->FollowCamera)
    {
        // Define a movement speed
        float MoveSpeed = 200.0f;

        /// Get the forward vector of the camera
        FVector ForwardVector = Orion->FollowCamera->GetForwardVector();

        // Zero out the Z component to make the camera move horizontally
        ForwardVector.Z = 0;

        // Normmalize the vector in case zeroing the z component has changed its length
        ForwardVector.Normalize();

        // Calculate the movement amount based on the input value, movement speed, and delta time
        FVector MoveAmount = ForwardVector * Value * MoveSpeed * GetWorld()->GetDeltaSeconds();

        // Predict the new location
        FVector PredictedLocation = Orion->FollowCamera->GetComponentLocation() + MoveAmount;

        // Move the camera to the new location
        Orion->FollowCamera->SetWorldLocation(PredictedLocation);
    }
}

void UBuildMode::MoveRight(float Value)
{
    if (bIsBuildModeActive && Orion && Orion->bRightMousePressed && Orion->FollowCamera)
    {
        // Define a movement speed
        float MoveSpeed = 200.0f;

        // Get the right vector of the camera
        FVector RightVector = Orion->FollowCamera->GetRightVector();

        // Zero out the Z component to make the movement horizontal
        RightVector.Z = 0;

        // Normalize the vector in case the Z component has changed length
        RightVector.Normalize();

        // Calculate the movement amount based on the input value, movement speed, and delta time
        FVector MoveAmount = RightVector * Value * MoveSpeed * GetWorld()->GetDeltaSeconds();

        // Predict the new location
        FVector PredictedLocation = Orion->FollowCamera->GetComponentLocation() + MoveAmount;

        // Move camera to new location
        Orion->FollowCamera->SetWorldLocation(PredictedLocation);
    }
}

void UBuildMode::Turn(float Value)
{
    if (bIsBuildModeActive)
    {
        if (Orion)
        {
            if (!Orion->bRightMousePressed)
            {
                // If the middle mouse button is not pressed, then exit the function early
                return;
            }

            // Scale the value to control the turn speed
            float ScaledValue = Value * TurnSpeed * GetWorld()->GetDeltaSeconds();

            // Calculate the desired rotation
            FRotator DesiredRotation(0.0f, ScaledValue, 0.0f);

            // Apply the rotation to Orion's follow camera
            Orion->FollowCamera->AddRelativeRotation(DesiredRotation);
        }
    }
}

void UBuildMode::Zoom(float Value)
{
    if (Orion)
    {
        float ZoomRange = 1000.0f;
        float MinCameraHeight = 600.0f;

        // Check if the camera has stopped moving and we haven't updated the initial height yet
        if (!Orion->bIsCameraMoving && !bHasUpdatedInitialCameraHeight)
        {
            InitialCameraHeight = Orion->FollowCamera->GetComponentLocation().Z;
            bHasUpdatedInitialCameraHeight = true;
        }

        // If the camera starts moving again, reset the flag
        if (Orion->bIsCameraMoving)
        {
            bHasUpdatedInitialCameraHeight = false;
        }

        // Check if the camera is moving
        if (Orion->bIsCameraMoving)
        {
            // If the camera is moving, exit the function early
            return;
        }
        // Check if the camera is moving
        if (Orion->bIsCameraMoving)
        {
            // If the camera is moving, exit the function early
            return;
        }

        if (Orion->FollowCamera)
        {
            // Define a zoom speed
            float ZoomSpeed = 800.0f;

            // Define a forward movement factor to control how much the camera moves forward
            float ForwardFactor = 400.0f;

            // Calculate the movement amount of the zoom
            FVector ZoomAmount = FVector(0, 0, Orion->FollowCamera->GetForwardVector().Z) * Value * ZoomSpeed * GetWorld()->GetDeltaSeconds();

            // Predict the new location of the zoom
            FVector PredictedZoomLocation = Orion->FollowCamera->GetComponentLocation() + ZoomAmount;

            // Clamp the height based on initial height
            float ClampedHeight = FMath::Clamp(PredictedZoomLocation.Z, FMath::Max(InitialCameraHeight - ZoomRange, MinCameraHeight), InitialCameraHeight);
            PredictedZoomLocation.Z = ClampedHeight;

            // Initialize forward movement amount
            FVector ForwardAmount = FVector::ZeroVector;
                
            if (ClampedHeight > InitialCameraHeight - 400.0f && ClampedHeight < InitialCameraHeight)
            {
                ForwardAmount = Orion->FollowCamera->GetForwardVector() * Value * ForwardFactor * GetWorld()->GetDeltaSeconds();
                ForwardAmount.Z = 0;
            }
            
            // Apply the combined zoom and forward movement
            FVector NewLocation = Orion->FollowCamera->GetComponentLocation() + ZoomAmount + ForwardAmount;
            NewLocation.Z = ClampedHeight;
            Orion->FollowCamera->SetWorldLocation(NewLocation);

            // Calculate the zoom ratio based on the current and max zoom levels
            float ZoomRatio = (ClampedHeight - (InitialCameraHeight - 450.0f)) / 450.0f;

            // Calculate the desired pitch changed based on zoom ratio
            float DesiredPitch = FMath::Lerp(-50.0f, -55.0f, ZoomRatio);

            // Apply the pitch change to orion's follow camera
            FRotator NewRotation(DesiredPitch, Orion->FollowCamera->GetRelativeRotation().Yaw, Orion->FollowCamera->GetRelativeRotation().Roll);
            Orion->FollowCamera->SetRelativeRotation(NewRotation);
        }
    }
}

void UBuildMode::ActivateBuildMode()
{
    // Spawn the selection box actor when build mode is activated
    if (World)
    {
        // SelectionBox = World->SpawnActor<ASelectionBox>();
    }

    bIsBuildModeActive = true;
    if (Orion && Orion->FollowCamera)
    {
        InitialCameraHeight = Orion->FollowCamera->GetComponentLocation().Z;
    }
}

void UBuildMode::DeactivateBuildMode()
{
    UE_LOG(LogBuildMode_Destruction, Warning, TEXT("Entering DeactivateBuildMode"));

    bIsBuildModeActive = false;

    if (SelectionBox)
    {
        SelectionBox->DestroyMesh();
        SelectionBox->Destroy();
        SelectionBox = nullptr;
        UE_LOG(LogBuildMode_Destruction, Log, TEXT("SelectionBox Destroyed"));
    }
}

bool UBuildMode::IsBuildModeActive()
{
    return bIsBuildModeActive;
}

void UBuildMode::LeftClick(bool bIsPressed)
{
    // Logging: Log the method call
    UE_LOG(LogTemp, Warning, TEXT("LeftClick Method Called_buildmode_leftclick"));

    // Update current state
    bIsLeftMouseCurrentlyPressed = bIsPressed;

    // If SelectionBox exists, pass the state to it
    if (IsValid(SelectionBox))
    {
        SelectionBox->LeftClick(bIsPressed);
    }

    // Only proceed to spawning if the mouse is pressed
    if (!bIsPressed)
    {
        return;
    }

    // Check if there's already an instance of SelectionBox
    if (IsValid(SelectionBox))
    {
        // If an instance already exists, do nothing
        UE_LOG(LogTemp, Warning, TEXT("SelectionBox Instance Already Exists. SelectionBox Spawn_buildmode = Request Rejected_buildmode_leftclick"));
        return;
    }

    // Get the player's viewpoint
    FVector ViewLocation;
    FRotator ViewRotation;
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

    // Get mouse position
    float MouseX, MouseY;
    PlayerController->GetMousePosition(MouseX, MouseY);

    // Deproject mouse position to world space
    FVector WorldDirection;
    FVector StartLocation;
    PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, StartLocation, WorldDirection);

    // Calculate the end location
    float RaycastDistance = 2000.0f;
    FVector EndLocation = StartLocation + (WorldDirection * RaycastDistance);
    
    // Perform raycast to detect where you've clicked
    FHitResult HitResult;
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility);

    if (bHit)
    {
        FVector ClickLocation = HitResult.Location;

            //FVector CameraForwardVector = ViewRotation.Vector();
            FRotator NewRotation = FRotator::ZeroRotator;
            NewRotation.Yaw += 0.0f;
            NewRotation.Pitch = 0.0f;
            NewRotation.Roll = 0.0f;

        // Spawn a new selection box actor at the click location
        SelectionBox = GetWorld()->SpawnActor<ASelectionBox>(ASelectionBox::StaticClass(), ClickLocation, NewRotation);

        // Log the action
        UE_LOG(LogTemp, Log, TEXT("SelectionBox Spawned at Location: %s_buildmode_leftclick"), *ClickLocation.ToString());

        // Notify SelectionBox of mouse click/release
        if (IsValid(SelectionBox))
        {
            SelectionBox->LeftClick(bIsPressed);

            UE_LOG(LogTemp, Log, TEXT("SelectionBox LeftClick Method Called_buildmode_leftclick"));
        }
    }
}

void UBuildMode::RightClick(bool bRightPressed)
{
    UE_LOG(LogTemp, Warning, TEXT("EnteringRightClick_buildmode_rightclick"));
    bIsRightMouseCurrentlyPressed = bRightPressed;
  
    if (IsValid(SelectionBox))
    {
        SelectionBox->RightClick(bRightPressed);
    } 
}
