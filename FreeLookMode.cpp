#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/CharacterLogic/FreeLookMode.h"
#include "GameFramework/SpringArmComponent.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/Characters/Orion.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Camera/CameraComponent.h"

UFreeLookMode::UFreeLookMode()
{
    bIsFreeLookModeActive = false;
}

void UFreeLookMode::ActivateFreeLookMode()
{
    bIsFreeLookModeActive = true;
}

void UFreeLookMode::DeactivateFreeLookMode()
{
    bIsFreeLookModeActive = false;
}

bool UFreeLookMode::IsFreeLookModeActive()
{
    return bIsFreeLookModeActive;
}

void UFreeLookMode::Turn(float Value)
{
    if (Orion && Orion->CameraBoom)
    {
        // Adjust turn speed
        float TurnSpeed = 10.0f;

        // Scale the value to control the turn speed
        float ScaledValue = Value * TurnSpeed * GetWorld()->GetDeltaSeconds();

        // Add yaw input to the controller
        FRotator NewRotation = Orion->CameraBoom->GetComponentRotation();
        NewRotation.Yaw += ScaledValue;
        Orion->CameraBoom->SetWorldRotation(NewRotation);
        Orion->AddControllerYawInput(ScaledValue);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Orion or Orion->FreeLookCameraBoom is null."));
    }
}
