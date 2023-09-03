	#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/ConstructorHelpers.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/CharacterLogic/BuildMode.h"

// Sets default values
ASelectionBox::ASelectionBox()
{
	UE_LOG(LogTemp, Log, TEXT("SelectionBox Constructor Called_selectionbox_constructor"));

	CurrentSelectedTag = TEXT("");
	TimeSinceLeftMousePressed = 0.0f;
	bIsLeftMouseButtonHeld = false;
	bIsLeftMousePressed = false;
	bIsDragging = false;

	// Logging: Variable Initializations
	UE_LOG(LogTemp, Log, TEXT("SelectionBox Variables Initialized_selectionbox_constructor"));

	// Logging: Debug log for current selected initialization
	UE_LOG(LogTemp, Log, TEXT("CurrentSelectedTag Initialized To_selectionbox_constructor: %s"), *CurrentSelectedTag);

	// Logging: Debug log for CurrentSelectedTag initialization
	UE_LOG(LogTemp, Log, TEXT("CurrentSelectedTag Initialized To_selectionbox_constructor: %s"), *CurrentSelectedTag);

	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the mesh component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// Set the default mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/GarudaRift/Meshes/Architecture/VFX/SelectionBox_garudarift.SelectionBox_garudarift'"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
		StaticMeshComponent->SetRelativeLocation(FVector(0.0f));
	}

	// Attach mesh to root component
	RootComponent = StaticMeshComponent;

	// Verify Wall's Orientation (if StaticMeshComponent is valid)
	UE_LOG(LogTemp, Log, TEXT("Wall Forward Vector_selectionbox_constructor: %s"), *StaticMeshComponent->GetForwardVector().ToString());
	UE_LOG(LogTemp, Log, TEXT("Wall Right Vector_selectionbox_constructor: %s"), *StaticMeshComponent->GetRightVector().ToString());

	UE_LOG(LogTemp, Log, TEXT(" Mesh Initialized in Constructor_selectionbox_constructor"));

	// Create spline component
	WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));

	// Attach the spline to its root
	WallSpline->SetupAttachment(RootComponent);

	// Log the spline component initilization
	UE_LOG(LogTemp, Log, TEXT("Spline Component Initialized_selectionbox_constructor"));

	// Get the bounding box of the static mesh
	FBox BoundingBox = StaticMeshComponent->GetStaticMesh()->GetBoundingBox();

	// Calculate the center of the wall
	FVector Center = BoundingBox.GetCenter();

	// Log the spline component initialization
	UE_LOG(LogTemp, Log, TEXT("WallSpline Component Initialized_selectionbox_constructor"));

	// Find the control point mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	UStaticMesh* SphereMesh = SphereMeshAsset.Object;

	// Get the right vector of the wall
	FVector WallRightVector = StaticMeshComponent->GetRightVector();

	// Define the offset from the center to the left and right sides
	float Offset = 15.f;

	// Calculate left and right positions for the control points
	FVector LeftPosition = Center - (WallRightVector * Offset);
	FVector RightPosition = Center + (WallRightVector * Offset);

	// Create the control point visualizations
	TArray<FVector> ControlPointLocations = { LeftPosition, RightPosition };
	for (int i = 0; i < ControlPointLocations.Num(); ++i)
	{
		FVector controlPointPosition = ControlPointLocations[i];

		// Logging: Verifying control point position
		UE_LOG(LogTemp, Log, TEXT("ControlPoint%d Position_selectionbox_constructor: %s"), i, *ControlPointLocations[i].ToString());

		UStaticMeshComponent* ControlPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(*FString::Printf(TEXT("ControlPointMesh%d"), i)));
		ControlPointMesh->SetupAttachment(RootComponent);
		ControlPointMesh->SetStaticMesh(SphereMesh);

		// Logging: Verify static mesh set
		UE_LOG(LogTemp, Log, TEXT("ControlPoint%d StaticMesh Set_selectionbox_constructor"), i);

		// Set the relative location to the left or right position
		ControlPointMesh->SetRelativeLocation(ControlPointLocations[i]);

		// Logging: Verify relative location set
		UE_LOG(LogTemp, Log, TEXT("ControlPoint%d Relative Location Set_selectionbox_constructor: %s"), i, *ControlPointLocations[i].ToString());

		// Set the mesh scale factor
		FVector ScaleFactor = FVector(0.1f, 0.1f, 0.1f);
		ControlPointMesh->SetWorldScale3D(ScaleFactor);

		// Logging: Verify mesh scale factor set
		UE_LOG(LogTemp, Log, TEXT("ControlPoint%d Scale Factor Set_selectionbox_constructor: %s"), i, *ScaleFactor.ToString());

		ControlPointMeshes.Add(ControlPointMesh);
	}

	UE_LOG(LogTemp, Log, TEXT("Control Points Initialized and Visible with Default Sphere Mesh_selectionbox_constructor"));
	UE_LOG(LogTemp, Log, TEXT("Mesh Initialized in Constructor_selectionbox_constructor"));
}

// Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Initialize a variable to keep track of whether the cursor is over a control point mesh
	bool bIsCursorOverControlPoint = false;

	// Initialize boolean flag
	bool bReachedOriginalLocation = false;

	// Time since left mouse was pressed
	if (bIsLeftMousePressed)
	{
		TimeSinceLeftMousePressed += DeltaTime;
	}

	// Get the current mouse location and convert it to a world space ray
	FVector2D MousePosition;
	if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		FVector worldLocation, worldDirection;
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->DeprojectMousePositionToWorld(worldLocation, worldDirection);

		// Get the gameplay controller
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		// Perform raycast to detect what was hovered
		FHitResult hitResult;
		FVector start = worldLocation;
		FVector end = ((worldDirection * 2000.0) + worldLocation);
		FCollisionQueryParams collisionParams;

		// Check if raycast hit something
		if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, collisionParams))
		{
			// Loop through the spline mesh array to check if hit component was one of the spline meshes
			for (int i = 0; i < ControlPointMeshes.Num(); ++i)
			{
				if (hitResult.GetComponent() == ControlPointMeshes[i])
				{
					bIsCursorOverControlPoint = true;

					// If mouse is clicked, set the selected control point mesh
					if (bIsLeftMousePressed)
					{
						SelectedControlPoint = ControlPointMeshes[i];
					}

					break;
				}
			}
		}

		// Change the cursor to a hand if it's over a control point mesh
		if (bIsCursorOverControlPoint)
		{
			PlayerController->CurrentMouseCursor = EMouseCursor::Hand;
		}
		else
		{
			PlayerController->CurrentMouseCursor = EMouseCursor::Default;
		}

		// Check if left mouse button is held for 0.5 seconds
		if (TimeSinceLeftMousePressed > 0.5 && bIsLeftMousePressed)
		{
			bIsLeftMouseButtonHeld = true;

			// Perform a new raycast to get the current mouse position
			if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, collisionParams))
			{	
				// Move the selected control point mesh with the mouse
				if (SelectedControlPoint && hitResult.bBlockingHit)
				{
					// Initialize StartingLocation when control point is first selected
					if (!bIsDragging)
					{
						StartingLocation = SelectedControlPoint->GetComponentLocation();
						bIsDragging = true;
					}
					
					FVector NewLocation = hitResult.Location;

					// Get the current world location of the control point
					FVector CurrentWorldLocation = SelectedControlPoint->GetComponentLocation();

					// Get the wall's normal vector
					FVector WallNormal = StaticMeshComponent->GetForwardVector();

					// Project location onto a planed defined by the wall's normal
					FVector PlanePoint = CurrentWorldLocation;
					FVector ProjectedLocation = FVector::PointPlaneProject(NewLocation, PlanePoint, WallNormal);

					// Determine the direction of the control point relative to the wall
					FVector Direction = (ProjectedLocation - StartingLocation).GetSafeNormal();

					// Smoothly Interpolate to the target world location
					FVector SmoothWorldLocation = FMath::VInterpTo(CurrentWorldLocation, ProjectedLocation, DeltaTime, 5.0f);

					// Move the control point to the SmoothWorldLocation
					SelectedControlPoint->SetWorldLocation(SmoothWorldLocation);

					// Logging: Verifty new control point location
					UE_LOG(LogTemp, Log, TEXT("New Projected Location of Control Point_selectionbox_tick: %s"), *SmoothWorldLocation.ToString());
				}
			}
		}
	}
	else
	{
		bIsLeftMouseButtonHeld = false;
	}
}

void ASelectionBox::LeftClick(bool bIsPressed)
{
	// Set the internal state of left click
	bIsLeftMousePressed = bIsPressed;

	// Logging: State of left bIsLeftMousePressed
	UE_LOG(LogTemp, Log, TEXT("LeftClick Called_selectionbox_leftclick: %s"), bIsLeftMousePressed ? TEXT("True") : TEXT("False"));
}

void ASelectionBox::DestroyMesh()
{
	// Check if static mesh component is null
	if (!StaticMeshComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("StaticMeshComponent is NULL in DestroyMesh_selectionbox_destroymesh"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("DestroyMesh Called_selectionbox_destroymesh"));

	// Check if static mesh component is still valid
	if (IsValid(this))
	{
		if (StaticMeshComponent)
		{
			// Detach the static mesh before destroying it
			StaticMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			StaticMeshComponent->DestroyComponent();

			// Debug log for successfully destroying the static mesh component
			UE_LOG(LogTemp, Log, TEXT("StaticMesh Destruction = Success_selectionbox_destroymesh"));
		}

		// Destroy the actor itself
		Destroy();

		// Debug log for successful destruction of the actor
		UE_LOG(LogTemp, Log, TEXT("SelectionBox Actor Destruction = Success_selectionbox_destroymesh"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SelectionBox Actor is not Valid or Already Destroyed_selectionbox_destroymesh"));
	}
}
