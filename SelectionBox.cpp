#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ASelectionBox::ASelectionBox()
{
	UE_LOG(LogTemp, Log, TEXT("SelectionBox Constructor Called_selectionbox_constructor"));

	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create spline component
	WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));

	// Attach the spline to its root
	WallSpline->SetupAttachment(RootComponent);

	// Add initial control points
	WallSpline->AddSplinePoint(FVector(0, 0, 0), ESplineCoordinateSpace::Local);
	WallSpline->AddSplinePoint(FVector(100, 0, 0), ESplineCoordinateSpace::Local);

	// Log the spline component initialization
	UE_LOG(LogTemp, Log, TEXT("WallSpline Component Initialized_selectionbox_constructor"));
	UE_LOG(LogTemp, Log, TEXT("Added Initial Control Points to WallSpline_selectionbox_constructor"));

	// Find the mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	UStaticMesh* SphereMesh = SphereMeshAsset.Object;

	// Create the control point visualizations
	for (int i = 0; i < WallSpline->GetNumberOfSplinePoints(); ++i)
	{
		UStaticMeshComponent* ControlPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(*FString::Printf(TEXT("ControlPointMesh%d"), i)));
		ControlPointMesh->SetupAttachment(RootComponent);
		ControlPointMesh->SetStaticMesh(SphereMesh);
		ControlPointMsesh->SetWorldlocation(WallSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
		ControlPointMeshes.Add(ControlPointMesh);
	}

	UE_LOG(LogTemp, Log, TEXT("Control Points Initialized and Visible with Default Sphere Mesh_selectionbox_constructor"));

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
	if (this && !IsPendingKill())
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
