#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/Characters/Orion.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/CharacterLogic/BuildMode.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/CharacterLogic/FreeLookMode.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Animation/AnimationAsset.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AOrion::AOrion()
{
    // Set this charcter to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    RotationSpeed = 20.0f;
    bShouldFaceCameraDirection = true;
    bIsExitingBuildMode = false;
    bIsEnteringBuildMode = false;
    bShouldReattachCamera = false;
    bShouldHideMesh = false;
    bIsLeftMouseButtonHeld = false;
    bIsLeftMousePressed = false;
    TimeSinceLeftMousePressed = 0.0f;

    // Disable automatic rotation to face the direction of movement
    GetCharacterMovement()->bOrientRotationToMovement = false;

    // Create a camera boom (pulls toward the player if there's a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Create a skeletal mesh
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    SkeletalMeshComponent->SetupAttachment(RootComponent);

    // Set the skeletal mesh
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshObj(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn"));
    if (SkeletalMeshObj.Succeeded())
    {
        SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshObj.Object);
    }

    // Set animation blueprint
    static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimObj(TEXT("/Game/GarudaRift/Blueprints/OrionAnim_BP.OrionAnim_BP"));
    if (AnimObj.Succeeded())
    {
        SkeletalMeshComponent->SetAnimInstanceClass(AnimObj.Object->GeneratedClass);
    }

    // Align the skeletal mesh with the capsule component
    SkeletalMeshComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

    FreeLookMode = NewObject<UFreeLookMode>(this, UFreeLookMode::StaticClass(), TEXT("FreeLookMode"));
    FreeLookMode->Orion = this;

    BuildMode = NewObject<UBuildMode>(this, UBuildMode::StaticClass(), TEXT("BuildMode"));
    BuildMode->Orion = this;
}

// Called when the game starts or when spawned
void AOrion::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AOrion::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Get current mouse location
    FVector2D mousePosition;
    GetWorld()->GetFirstPlayerController()->GetMousePosition(mousePosition.X, mousePosition.Y);

    // Convert the mouse position to a world space ray
    FVector worldLocation, worldDirection;
    GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(mousePosition.X, mousePosition.Y, worldLocation, worldDirection);

    // Perform raycast to detect what was hovered
    FHitResult hitResult;
    FVector start = worldLocation;
    FVector end = ((worldDirection * 2000.0f) + worldLocation);
    FCollisionQueryParams collisionParams;

    // Check if raycast hit something
    if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, collisionParams))
    {
        // Logging: Debug log for hitting something
        UE_LOG(LogTemp, Log, TEXT("Raycast Hit Something_orion_tick: %s"), *hitResult.GetActor()->GetName());

        // Get the hit component
        UPrimitiveComponent* HitComponent = hitResult.GetComponent();

        // Check if the hit component is valid and has the ControlPoint tag
        if (HitComponent != nullptr && HitComponent->ComponentHasTag(FName("ControlPoint")))
        {
            // Logging: Debug log for hovering over over a control point mesh
            UE_LOG(LogTemp, Log, TEXT("Raycast Hit Control Point Mesh_orion_tick: %s"), *HitComponent->GetName());

            // Change the cursor to a hand
            GetWorld()->GetFirstPlayerController()->CurrentMouseCursor = EMouseCursor::Hand;
        }
        else
        {
            // Logging: Debug log for hit component being null or not having the control point tag
            UE_LOG(LogTemp, Error, TEXT("Hit Component is Null or Does Not Have ControlPoint Tag_orion_tick"));

            // Revert the cursror back to default state
            GetWorld()->GetFirstPlayerController()->CurrentMouseCursor = EMouseCursor::Default;
        }
    }

    if (bIsLeftMousePressed)
    {
        TimeSinceLeftMousePressed += DeltaTime;

        if (TimeSinceLeftMousePressed >= 3.0f)
        {
            // Handle left mouse being held for at least 3 seconds
            bIsLeftMouseButtonHeld = true;
            if (BuildMode && BuildMode->IsBuildModeActive())
            {
                BuildMode->LeftMouseDrag(InitialLeftClickLocation);
            }
        }
    }
    else
    {
        // Reset the variables
        TimeSinceLeftMousePressed = 0.0f;
        bIsLeftMouseButtonHeld = false;
    }

    if (bIsCameraMoving)
    {
        // Current camera location
        FVector CurrentLocation = FollowCamera->GetComponentLocation();

        // Interpolate camera position
        FVector NewLocation = FMath::VInterpTo(CurrentLocation, CameraTargetLocation, DeltaTime, 3.0f);

        // Set the new camera location
        FollowCamera->SetWorldLocation(NewLocation);

        // Interpolate the camera rotation
        FRotator NewRotation = FMath::RInterpTo(FollowCamera->GetComponentRotation(), CameraTargetRotation, DeltaTime, 3.0f);

        // Set the new camera rotation
        FollowCamera->SetWorldRotation(NewRotation);

        // Check if the camera has reached its target location and rotation
        if (FollowCamera->GetComponentLocation().Equals(CameraTargetLocation, 1.0f) && FollowCamera->GetComponentRotation().Equals(CameraTargetRotation, 1.0f))
        {
            bIsCameraMoving = false;

            // If in build mode and the camera has stopped moving, hide the mesh
            if (BuildMode->IsBuildModeActive())
            {
                SkeletalMeshComponent->SetVisibility(!bShouldHideMesh);
            }

            // If in build mode and the camera has stopped moving, show the mouse cursor
            if (BuildMode->IsBuildModeActive())
            {
                SkeletalMeshComponent->SetVisibility(false);

                // Show mouse cuursor
                APlayerController* PlayerController = Cast<APlayerController>(GetController());
                if (PlayerController)
                {
                    PlayerController->bShowMouseCursor = true;
                    PlayerController->bEnableClickEvents = true;
                    PlayerController->bEnableMouseOverEvents = true;
                }
            }

            // If the camera should be reattached to the boom, do it
            if (bShouldReattachCamera)
            {
                FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetIncludingScale);
                bShouldReattachCamera = false;

                CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, SavedTargetArmLength, DeltaTime, 3.0f);
            }
        }
    }
}

// Called to bind functionality to input
void AOrion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AOrion::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AOrion::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AOrion::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AOrion::LookUp);
    PlayerInputComponent->BindAxis("Zoom", this, &AOrion::Zoom);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AOrion::Jump);
    PlayerInputComponent->BindAction("FreeLook", IE_Pressed, this, &AOrion::ActivateFreeLookMode);
    PlayerInputComponent->BindAction("FreeLook", IE_Released, this, &AOrion::DeactivateFreeLookMode);
    PlayerInputComponent->BindAction("EnterBuildMode", IE_Pressed, this, &AOrion::EnterBuildMode);
    PlayerInputComponent->BindAction("ExitBuildMode", IE_Pressed, this, &AOrion::ExitBuildMode);
    PlayerInputComponent->BindAction("MiddleMousePressed", IE_Pressed, this, &AOrion::ToggleAutoRun);
    PlayerInputComponent->BindAction("MiddleMouseReleased", IE_Released, this, &AOrion::MiddleMouseReleased);
    PlayerInputComponent->BindAction("RightMouse", IE_Pressed, this, &AOrion::RightMousePressed);
    PlayerInputComponent->BindAction("RightMouse", IE_Released, this, &AOrion::RightMouseReleased);
    PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &AOrion::LeftMousePressed);
    PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &AOrion::LeftMouseReleased);
}

void AOrion::MiddleMousePressed()
{
    bIsMiddleMousePressed = true;
}

void AOrion::MiddleMouseReleased()
{
    bIsMiddleMousePressed = false;
}

void AOrion::RightMousePressed()
{
    bIsRightMousePressed = true;

    if (BuildMode && BuildMode->IsBuildModeActive())
    {
        BuildMode->UpdateCursorVisibility();
    }
}

void AOrion::RightMouseReleased()
{
    bIsRightMousePressed = false;

    if (BuildMode && BuildMode->IsBuildModeActive())
    {
        BuildMode->UpdateCursorVisibility();
    }
}

void AOrion::LeftMousePressed()
{
    // Logging: Debug log for entering the LeftMousePressed method
    UE_LOG(LogTemp, Log, TEXT("LeftMousePressed Called_orion_leftmousepressed"));

    bIsLeftMousePressed = true;

    // Get the current mouse position and store it in a variable
    GetWorld()->GetFirstPlayerController()->GetMousePosition(InitialLeftClickLocation.X, InitialLeftClickLocation.Y);

    // Logging: Debug log for storing initial left click location
    UE_LOG(LogTemp, Log, TEXT("Initial Left Click Location_orion_leftmousepressed: X=%f, Y=%f"), InitialLeftClickLocation.X, InitialLeftClickLocation.Y);
    
    if (BuildMode && BuildMode->IsBuildModeActive())
    {
        BuildMode->LeftClick();

        BuildMode->LeftMouseDrag(InitialLeftClickLocation);
    }

    // Logging: Debug log for exiting left mouse pressed method
    UE_LOG(LogTemp, Log, TEXT("LeftMousePressed Exited_orion_leftmousepressed"));
}

void AOrion::LeftMouseReleased()
{
    bIsLeftMousePressed = false;
}

void AOrion::ToggleAutoRun()
{
    bIsAutoRunning = !bIsAutoRunning;
}

void AOrion::FaceRotation(FRotator NewControlRotation, float DeltaTime)
{
    // Only update the character's rotation if not in freelookmode and if the character should face the camera direction
    if (!FreeLookMode->IsFreeLookModeActive() && bShouldFaceCameraDirection)
    {
        // Call the base class version of facerotation to handle any functionality it might have
        Super::FaceRotation(NewControlRotation, DeltaTime);
    }
}

void AOrion::ActivateFreeLookMode()
{
    // If in build mode, do not activate free look mode
    if (BuildMode && BuildMode->IsBuildModeActive())
    {
        return;
    }
    FreeLookMode->ActivateFreeLookMode();
    bShouldFaceCameraDirection = false;
}

void AOrion::DeactivateFreeLookMode()
{
    FreeLookMode->DeactivateFreeLookMode();
}

void AOrion::EnterBuildMode()
{
    // If build mode is already active, don't do anything
    if (BuildMode->IsBuildModeActive())
    {
        return;
    }

    // Store the original forward vector of the camera boom
    OriginalBoomForwardVector = FollowCamera->GetForwardVector();

    // Store the current rotation of the camera boom
    BoomRotationBeforeBuildMode = CameraBoom->GetComponentRotation();

    // Remember the current camera rotation
    CameraRotationBeforeBuildMode = FollowCamera->GetComponentRotation();

    // Remember current camera world transform for when we exit build mode
    CameraTransformBeforeBuildMode = FollowCamera->GetComponentTransform();

    // Store the current arm length before entering build mode
    SavedTargetArmLength = CameraBoom->TargetArmLength;

    // Detach the camera from the boom
    FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    // Position the camera 500cm direction above the character's current location
    CameraTargetLocation = GetActorLocation() + FVector(0, 0, 500.f);

    // Set the target rotation for the camera
    CameraTargetRotation = FRotator(-55.0f, FollowCamera->GetComponentRotation().Yaw, FollowCamera->GetComponentRotation().Roll);

    // Start moving the camera
    bIsCameraMoving = true;

    // Set build mode active
    BuildMode->ActivateBuildMode();

    // Show mouse cursor
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = false;
        PlayerController->bEnableClickEvents = false;
        PlayerController->bEnableMouseOverEvents = false;
    }

    bShouldReattachCamera = false;
    bShouldHideMesh = true;
}

void AOrion::ExitBuildMode()
{
    // If build mode is not active, don't do anything
    if (!BuildMode->IsBuildModeActive())
    {
        return;
    }

    // Calculate the offset based on target arm length and the boom's backward direction
    FVector CameraOffset = OriginalBoomForwardVector * (-SavedTargetArmLength);

    // Set the target location for the camera by adding the offset to the boom's world location
    CameraTargetLocation = CameraBoom->GetComponentLocation() + CameraOffset;

    // Set the target rotation of the camera based on the current boom rotation
    CameraTargetRotation = CameraRotationBeforeBuildMode;

    // Start moving the camera
    bIsCameraMoving = true;

    // Deactivate build mode
    BuildMode->DeactivateBuildMode();

    // Hide mouse cursor
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = false;
        PlayerController->bEnableClickEvents = false;
        PlayerController->bEnableMouseOverEvents = false;
    }

    // Unhide the mesh immediately when exiting build mode
    SkeletalMeshComponent->SetVisibility(true);

    bShouldReattachCamera = true;
}

void AOrion::MoveForward(float Value)
{
    if (BuildMode && BuildMode->IsBuildModeActive())
    {
        BuildMode->MoveForward(Value);
        return;
    }

    // If in build mode or the camera is moving, don't execute the function
    if (BuildMode->IsBuildModeActive() || bIsCameraMoving)
    {
        bIsAutoRunning = false;
    }

    // Turn off auto-run if moving
    if (Value != 0.0f)
    {
        bIsAutoRunning = false;
    }

    // If auto-run is enabled and no manual forward movement is detected, set the movement value
    if (bIsAutoRunning && Value == 0.0f)
    {
        Value = 1.0;
    }

    if ((Controller != NULL) && (Value != 0.0f) && !FreeLookMode->IsFreeLookModeActive())
    {
        // Find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // Get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);

        // Set the flag to update the character's rotation to face the camera's direction
        bShouldFaceCameraDirection = true;
    }
}

void AOrion::MoveRight(float Value)
{
    if (BuildMode && BuildMode->IsBuildModeActive())
    {
        BuildMode->MoveRight(Value);
        return;
    }

    // If in build mode or the camera is moving, don't execute the function
    if (BuildMode->IsBuildModeActive() || bIsCameraMoving)
    {
        return;
    }

    // Turn off auto-run if moving
    if (Value != 0.0f)
    {
        bIsAutoRunning = false;
    }

    if ((Controller != NULL) && (Value != 0.0f) && !FreeLookMode->IsFreeLookModeActive())
    {
        // Find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // Get right vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);

        // Set the flag to update the character's rotation to face the camera direction
        bShouldFaceCameraDirection = true;
    }
}

void AOrion::Turn(float Value)
{
    // If in build mode or the camera is moving, don't execute the function
    if (bIsCameraMoving)
    {
        return;
    }

    if (BuildMode && BuildMode->IsBuildModeActive())
    {
        BuildMode->Turn(Value);
        return;
    }

    if (FreeLookMode && FreeLookMode->bIsFreeLookModeActive)
    {
        // If in free look mode, control the free look camera
        FreeLookMode->Turn(Value);
    }
    else
    {
        // Adjust the turn speed
        float TurnSpeed = 10.0f;

        // Scale the value to control the turn speed
        float ScaledValue = Value * TurnSpeed * GetWorld()->GetDeltaSeconds();

        // Add yaw input to the controller
        AddControllerYawInput(ScaledValue);
        bUseControllerRotationYaw = true;

        CameraTransformBeforeBuildMode = FollowCamera->GetComponentTransform();
    }
}

void AOrion::LookUp(float Value)
{
    // If in build mode or the camera is moving, don't execute the function
    if (BuildMode->IsBuildModeActive() || bIsCameraMoving)
    {
        return;
    }

    // Adjust the look up speed
    float LookUpSpeed = 10.0f;

    // Scale the value to control the look up speed
    float ScaledValue = Value * LookUpSpeed * GetWorld()->GetDeltaSeconds();

    // Calculate expected new pitch
    float NewPitch = GetControlRotation().Pitch - ScaledValue;

    if (NewPitch > 180.0f)
    {
        NewPitch = NewPitch - 360.0f;
    }

    // If the new pitch would go outside the allowed range, don't apply the input
    if (NewPitch < -75.0f && ScaledValue > 0 || (NewPitch > 20.0f && ScaledValue < 0))
    {
        return;
    }

    // Apply delta pitch to the controller
    AddControllerPitchInput(ScaledValue);

    CameraTransformBeforeBuildMode = FollowCamera->GetComponentTransform();
}

void AOrion::Zoom(float Value)
{
    // If in build mode, don't execute the function
    if (BuildMode->IsBuildModeActive())
    {
        // If in build mode, control the zoom using buildmode's zoom function
        BuildMode->Zoom(Value);
        return;
    }

    CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength - Value * 10.0f, 200.0f, 500.0f);
}

void AOrion::Jump()
{
    // If in build mode or the camera is moving, don't execute the function
    if (BuildMode->IsBuildModeActive() || bIsCameraMoving)
    {
        return;
    }

    if (ACharacter::CanJump() && !FreeLookMode->IsFreeLookModeActive())
    {
        ACharacter::Jump();
    }
}
