#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ASelectionBox::ASelectionBox()
{
	UE_LOG(LogTemp, Log, TEXT("SelectionBox Constructor Called_selectionbox_constructor"));

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

	UE_LOG(LogTemp, Log, TEXT(" Mesh Initialized in Constructor_selectionbox_constructor"));

	// Create spline component
	WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));

	// Attach the spline to its root
	WallSpline->SetupAttachment(RootComponent);

	// Log the spline component initilization
	UE_LOG(LogTemp, Log, TEXT("Spline Component Initialized_selectionbox_constructor"));

	// Get the bounding box of the static mesh
	FBox BoundingBox = StaticMeshComponent->GetStaticMesh()->GetBoundingBox();

	// Get the min and max extents of the bounding box
	FVector MinExtent = BoundingBox.Min;
	FVector MaxExtent = BoundingBox.Max;

	// Log the bounding box extents
	UE_LOG(LogTemp, Log, TEXT("BoundingBox MinExtent: %s, MaxExtent: %s_selectionbox_constructor"), *MinExtent.ToString(), *MaxExtent.ToString());

	// Calculate the locations of the end faces
	FVector EndFace1Location = FVector(MinExtent.X, (MinExtent.Y + MaxExtent.Y) / 2, (MinExtent.Z + MaxExtent.Z) / 2);
	FVector EndFace2Location = FVector(MaxExtent.X, (MinExtent.Y + MaxExtent.Y) / 2, (MinExtent.Z + MaxExtent.Z) / 2);

	// Add control points to the end face locations
	WallSpline->AddSplinePoint(EndFace1Location, ESplineCoordinateSpace::Local);
	WallSpline->AddSplinePoint(EndFace2Location, ESplineCoordinateSpace::Local);

	// Log the end face locations
	UE_LOG(LogTemp, Log, TEXT("EndFace1Location: %s, EndFace2Location: %s_selectionbox_constructor"), *EndFace1Location.ToString(), *EndFace2Location.ToString());

	// Log the spline component initialization
	UE_LOG(LogTemp, Log, TEXT("WallSpline Component Initialized_selectionbox_constructor"));
	UE_LOG(LogTemp, Log, TEXT("Added Initial Control Points to WallSpline_selectionbox_constructor"));

	// Find the control point mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	UStaticMesh* SphereMesh = SphereMeshAsset.Object;

	// Get the right vector of the wall
	FVector WallRightVector = StaticMeshComponent->GetRightVector();

	// Create the control point visualizations
	TArray<FVector> ControlPointLocations = { EndFace1Location, EndFace2Location };
	for (int i = 0; i < ControlPointLocations.Num(); ++i)
	{
		UStaticMeshComponent* ControlPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(*FString::Printf(TEXT("ControlPointMesh%d"), i)));
		ControlPointMesh->SetupAttachment(RootComponent);
		ControlPointMesh->SetStaticMesh(SphereMesh);
		ControlPointMesh->SetRelativeLocation(ControlPointLocations[i]);

		// Set the rotation based on the wall's right vector
		FRotator Rotation = WallRightVector.Rotation();

		ControlPointMesh->SetRelativeRotation(Rotation);

		// Set the mesh scale factor
		FVector ScaleFactor = FVector(0.1f, 0.1f, 0.1f);
		ControlPointMesh->SetWorldScale3D(ScaleFactor);

		ControlPointMeshes.Add(ControlPointMesh);
	}

	UE_LOG(LogTemp, Log, TEXT("Control Points Initialized and Visible with Default Sphere Mesh_selectionbox_constructor"));
	UE_LOG(LogTemp, Log, TEXT("Mesh Initialized in Constructor_selectionbox_constructor"));
}

// Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("BeginPlay_selectionbox_beginplay"));
}

// Called every frame
void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
