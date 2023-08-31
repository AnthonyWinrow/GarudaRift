#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
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

	// Time since left mouse was pressed
	if (bIsLeftMousePressed)
	{
		TimeSinceLeftMousePressed += DeltaTime;
	}

	// Check if left mouse button is held for 0.5 seconds
	if (TimeSinceLeftMousePressed > -0.5f && bIsLeftMouseButtonHeld == false)
	{
		bIsLeftMouseButtonHeld = true;

		// Logging: State of left mouse button held
		UE_LOG(LogTemp, Log, TEXT("Left Mouse Button Held for 0.5 Seconds_selectionbox_tick"));
	}

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

		// Loop through the spline mesh array to check if the hit component was one of the spline meshes
		for (int i = 0; i < ControlPointMeshes.Num(); ++i)
		{
			if (HitComponent == ControlPointMeshes[i])
			{
				// Logging: Identify the clicked spline mesh
				UE_LOG(LogTemp, Log, TEXT("Clicked on SplineMesh_selectionbox_tick %d"), i);
				break;
			}
		}
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
