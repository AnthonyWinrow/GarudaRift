#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/ConstructorHelpers.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/CharacterLogic/BuildMode.h"

// Sets default values
ASelectionBox::ASelectionBox()
{
	UE_LOG(LogTemp, Log, TEXT("SelectionBox Constructor Called_selectionbox_constructor"));

	TimeSinceLeftMousePressed = 0.0f;
	bIsLeftMouseButtonHeld = false;
	bIsLeftMousePressed = false;
	bIsDragging = false;
	bHasCapturedInitialLocation = false;

	// Logging: Variable Initializations
	UE_LOG(LogTemp, Log, TEXT("SelectionBox Variables Initialized_selectionbox_constructor"));

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

	// Create the control point visualizations for ControlPointPointMesh0
	ControlPointMesh0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControlPointMesh0"));
	ControlPointMesh0->SetupAttachment(RootComponent);
	ControlPointMesh0->SetStaticMesh(SphereMesh);
	ControlPointMesh0->SetRelativeLocation(LeftPosition);
	FVector ScaleFactor0 = FVector(0.1f, 0.1f, 0.1f);
	ControlPointMesh0->SetWorldScale3D(ScaleFactor0);

	ControlPointMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControlPointMesh1"));
	ControlPointMesh1->SetupAttachment(RootComponent);
	ControlPointMesh1->SetStaticMesh(SphereMesh);
	ControlPointMesh1->SetRelativeLocation(RightPosition);
	FVector ScaleFactor1 = FVector(0.1f, 0.1, 0.1f);
	ControlPointMesh1->SetWorldScale3D(ScaleFactor1);

	// Logging: Verify control point initialization
	UE_LOG(LogTemp, Log, TEXT("Control Points Initialized and Visible with Default Sphere Mesh_selectionbox_constructor"));
}

// Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("BeginPlay Now Called_selectionbox_beginplay"));

	// Store the location of ControlPointMesh0
	InitialLocationOfControlPointMesh0 = ControlPointMesh0->GetComponentLocation();

	UE_LOG(LogTemp, Log, TEXT("InitialLocationOfControlPoint0_selectionbox_beginplay: %s"), *InitialLocationOfControlPointMesh0.ToString());

	InitialLocationOfControlPointMesh1 = ControlPointMesh1->GetComponentLocation();

	UE_LOG(LogTemp, Log, TEXT("InitialLocationOfControlPoint1_selectionbox_beginplay: %s"), *InitialLocationOfControlPointMesh1.ToString());
}

// Called every frame
void ASelectionBox::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Log, TEXT("Tick Now Called_selectionbox_tick"));

	// Logging Verify initial location of ControlPointMesh0
	UE_LOG(LogTemp, Log, TEXT("InitialLocationOfControlPointMesh0_selectionbox_tick: %s"), *InitialLocationOfControlPointMesh0.ToString());

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
			// Check if hit component was one of the control point meshes
			if (hitResult.GetComponent()->IsA(UStaticMeshComponent::StaticClass()))
			{
				UStaticMeshComponent* hitMesh = Cast<UStaticMeshComponent>(hitResult.GetComponent());

				if (hitMesh == ControlPointMesh0 || hitMesh == ControlPointMesh1)
				{
					UE_LOG(LogTemp, Log, TEXT("Cursor is Over a Control Point"));
					bIsCursorOverControlPoint = true;

					// If mouse is clicked, set the selected control point mesh
					if (bIsLeftMousePressed)
					{
						UE_LOG(LogTemp, Log, TEXT("Control Point Selected_selectionbox_tick"));
						SelectedControlPoint = hitMesh;
					}
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

		// Hide the mouse cursor when dragging
		if (bIsDragging)
		{
			PlayerController->bShowMouseCursor = false;
		}
		else
		{
			PlayerController->bShowMouseCursor = true;
		}

		// Check if left mouse button is held for 0.5 seconds
		if (TimeSinceLeftMousePressed > 0.5 && bIsLeftMousePressed)
		{
			bIsLeftMouseButtonHeld = true;

			// Perform a new raycast to get the current mouse position
			if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, collisionParams))
			{
				// Move the selected control point mesh with the mouse
				if ((SelectedControlPoint == ControlPointMesh0) || (SelectedControlPoint == ControlPointMesh1) && hitResult.bBlockingHit)
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
					FVector SmoothWorldLocation = FMath::VInterpTo(CurrentWorldLocation, ProjectedLocation, DeltaTime, 3.0f);

					// Move the control point to the SmoothWorldLocation
					SelectedControlPoint->SetWorldLocation(SmoothWorldLocation);

					static float InitialDotProduct = FLT_MAX;

					// Clamp ControlPointmeshes
					if (SelectedControlPoint == ControlPointMesh0)
					{
						// If this is the first time we're dragging this control point, store its initial location
						if (!bIsDragging)
						{
							InitialLocationOfControlPointMesh0 = SelectedControlPoint->GetComponentLocation();
						}

						FVector WallRight = StaticMeshComponent->GetRightVector();
						FVector CurrentLocation = SelectedControlPoint->GetComponentLocation();

						// Logging: Verify WallRight and CurrentLocation
						UE_LOG(LogTemp, Log, TEXT("WallRight: %s, CurrentLocation_selectionbox_tick: %s"), *WallRight.ToString(), *CurrentLocation.ToString());

						// Calculate the distance from the initial location
						float Distance = FVector::Dist(InitialLocationOfControlPointMesh0, CurrentLocation);

						// Determine the direction of movement
						FVector MovementDirection = (CurrentLocation - InitialLocationOfControlPointMesh0).GetSafeNormal();

						// Logging: Verify WallRight, MovementDirection, and ProjectedLocation
						UE_LOG(LogTemp, Log, TEXT("WallRight: %s, MovementDirection: %s, ProjectedLocation_selectionbox_tick: %s"), *WallRight.ToString(), *MovementDirection.ToString(), *ProjectedLocation.ToString());

						// Calculate DotProduct
						float DotProduct = FVector::DotProduct(MovementDirection, WallRight);

						// Calculate the vector from the wall to the control point
						FVector WallToControlPoint = CurrentLocation - InitialLocationOfControlPointMesh0;

						// Calculate the dot product to check alighnment
						float AlignmentDotProduct = FVector::DotProduct(WallToControlPoint.GetSafeNormal(), WallNormal);

						// Logging: Verify dot product
						UE_LOG(LogTemp, Log, TEXT("DotProduct_selectionbox_tick: %f"), DotProduct);

						// If the control point is behind the wall, snap it back
						if (AlignmentDotProduct < 0)
						{
							UE_LOG(LogTemp, Warning, TEXT("Control Point is Behind Wall, Resetting_selectionbox_tick"));
							SelectedControlPoint->SetWorldLocation(InitialLocationOfControlPointMesh0);
						}
						// Only allow movement to the left of the wall
						else if (DotProduct < 0)
						{
							// Clamp the distance so that it doesn't go past initial location
							float ClampedDistance = FMath::Max(Distance, 0.0f);

							// Calculate the new clamped location
							FVector ClampedLocation = InitialLocationOfControlPointMesh0 + WallRight * -ClampedDistance;

							UE_LOG(LogTemp, Log, TEXT("Moving Control Point to Clamped Location_selectionbox_tick: %s"), *ClampedLocation.ToString());

							// Set new clamped location
							SelectedControlPoint->SetWorldLocation(ClampedLocation);
						}
						else
						{
							// Reset to initial location if trying to move to the right
							SelectedControlPoint->SetWorldLocation(InitialLocationOfControlPointMesh0);
						}
					}
					else if (SelectedControlPoint == ControlPointMesh1)
					{
						if (!bIsDragging)
						{
							InitialLocationOfControlPointMesh1 = SelectedControlPoint->GetComponentLocation();
						}

						FVector WallRight = StaticMeshComponent->GetRightVector();
						FVector CurrentLocation = SelectedControlPoint->GetComponentLocation();

						// Calculate the distance from initial location
						float Distance = FVector::Dist(InitialLocationOfControlPointMesh1, CurrentLocation);

						// Determine the direction of movement
						FVector MovementDirection = (CurrentLocation - InitialLocationOfControlPointMesh1).GetSafeNormal();

						// Calculate dot product
						float DotProduct = FVector::DotProduct(MovementDirection, WallRight);

						// If the control point is behind the wall, snap it back
						if (DotProduct < 0)
						{
							SelectedControlPoint->SetWorldLocation(InitialLocationOfControlPointMesh1);
						}
						// Only allow movement to the right of the wall
						else if (DotProduct > 0)
						{
							// Clamp the distance so that it doesn't go past initial position
							float ClampedDistance = FMath::Max(Distance, 0.0f);

							// Calculate new clamped location
							FVector ClampedLocation = InitialLocationOfControlPointMesh1 + WallRight * ClampedDistance;

							// Set new clamped location
							SelectedControlPoint->SetWorldLocation(ClampedLocation);
						}
					}

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

	if (bIsLeftMousePressed && !bHasCapturedInitialLocation)
	{
		// Capture initial location only once
		bHasCapturedInitialLocation = true;
		InitialLocationOfControlPointMesh0 = ControlPointMesh0->GetComponentLocation();
		UE_LOG(LogTemp, Log, TEXT("Captured Initial Location_selectionbox_leftclick: %s"), *InitialLocationOfControlPointMesh0.ToString());
	}

	if (!bIsLeftMousePressed)
	{
		// Reset dragging state when the button is released
		bIsDragging = false;
	}
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
