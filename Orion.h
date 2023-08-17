#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/CharacterLogic/FreeLookMode.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/CharacterLogic/BuildMode.h" 
#include "Orion.generated.h"

class UGroundMode;

UCLASS()
class GARUDARIFT_API AOrion : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AOrion();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Camera boom positioning the camera behind the character
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class USpringArmComponent* CameraBoom;

    // Free look camera boom
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        class USpringArmComponent* FreeLookCameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        class UCameraComponent* FreeLookCamera;

    // Follow camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UCameraComponent* FollowCamera;

    // Handles moving forward and backward across all modes
    void MoveForward(float Value);

    // Handles moving right and left
    void MoveRight(float Value);

    // Handles camera movement right and left
    void Turn(float Value);

    // Handles camera movement up and down
    void LookUp(float Value);

    // Handles camera zoom
    void Zoom(float Value);

    // Handles jumping
    void Jump();
    
    // AutoRun methods
    void ToggleAutoRun();

    // Handles middle mouse button
    void MiddleMousePressed();
    void MiddleMouseReleased();

    // Handles right mouse button
    void RightMousePressed();
    void RightMouseReleased();

    // Handles left mouse button
    void LeftMousePressed();
    void LeftMouseReleased();
    float TimeSinceLeftMousePressed;
    bool bIsLeftMouseButtonHeld;
    bool bIsLeftMousePressed;

    // Handles FreeLookMode
    void ActivateFreeLookMode();
    void DeactivateFreeLookMode();

    // Function Variables
    bool bIsCameraMoving;
    bool bIsExitingBuildMode;
    bool bIsEnteringBuildMode;
    bool bIsZoomingIn;
    bool bIsMiddleMousePressed;
    bool bIsRightMousePressed;
    bool bShouldReattachCamera;
    bool bShouldHideMesh;
    bool bIsAutoRunning;
    FRotator CameraStartRotation;
    FRotator BoomRotationBeforeBuildMode;
    FVector CameraStartLocation;
    FVector CameraTargetLocation;
    FVector CurrentLocalLocation;
    FVector OriginalBoomForwardVector;
    FVector DragStartLocation;
    float CameraMoveStartTime;
    float CameraMoveDuration;
    float LastAutoRunDisableTime;

    // Camera Storage
    FVector InitialCameraRelativeLocation;
    FVector InitialCameraLocation;
    FRotator InitialCameraRotation;
    FRotator CameraTargetRotation;
    FTransform CameraTransformBeforeBuildMode;
    FRotator CameraRotationBeforeBuildMode;

    // Indicates whether the character should update its rotation to face the direction of the camera
    bool bShouldFaceCameraDirection;

    void FaceRotation(FRotator NewControlRotation, float DeltaTime);

    // Skeletal mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
        class USkeletalMeshComponent* SkeletalMeshComponent;

    // Rotation Speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        float RotationSpeed;

    // FreeLookMode
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Free Look Mode")
        class UFreeLookMode* FreeLookMode;

    // BuildMode
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildMode")
        class UBuildMode* BuildMode;

    // Handles BuildMode
    void EnterBuildMode();
    void ExitBuildMode();

private:
    // Stores the desired camera arm length when exiting build mode
    float DesiredArmLength;

    // Stores the target arm length before entering build mode
    float SavedTargetArmLength;
};
