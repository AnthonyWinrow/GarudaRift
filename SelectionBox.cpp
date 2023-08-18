#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

// Sets default values
ASelectionBox::ASelectionBox()
{
	// Log: Constructor Called
	UE_LOG(LogTemp, Warning, TEXT("Mesh SelectionBox Constructor Called_selectionbox construct"));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create static mesh component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetMobility(EComponentMobility::Movable);
	SetRootComponent(StaticMeshComponent);

	// Log: Static mesh created
	UE_LOG(LogTemp, Warning, TEXT("StaticMesh Component Created_selectionbox construct"));

	// Load static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(MeshAsset.Object);

		// Log: Mesh loaded successfully
		UE_LOG(LogTemp, Warning, TEXT("StaticMesh Loaded Successfully_selectionbox construct"));
	}
	else
	{
		// Log: Mesh not found
		UE_LOG(LogTemp, Warning, TEXT("StaticMesh Not Found_selectionbox construct"));
	}

	// Load material
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("MaterialInstanceConstant'/Game/GarudaRift/Materials/VFx/BuildSelection.BuildSelection'"));
	if (MaterialAsset.Succeeded())
	{
		StaticMeshComponent->SetMaterial(0, MaterialAsset.Object);

		// Log: Material loaded succesfully
		UE_LOG(LogTemp, Warning, TEXT("Material Loaded Successfully_selectionbox construct"));
	}
	else
	{
		// Log: Material Not Found
		UE_LOG(LogTemp, Warning, TEXT("Material Not Found_selectionbox construct"));
	}
	// Set default location
	FVector Location(1.f, 1.f, 1.f);
	StaticMeshComponent->SetRelativeLocation(Location);

	// Log the set location
	UE_LOG(LogTemp, Warning, TEXT("StaticMeshLocation Set: %s_selectionbox construct"), *Location.ToString());

	// Set the world scale of the static mesh component
	FVector Scale(1.f, 1.f, 1.f);
	StaticMeshComponent->SetWorldScale3D(Scale);

	// Set the visibility of the static mesh component
	StaticMeshComponent->SetVisibility(true);

	// Log the visibility set
	UE_LOG(LogTemp, Warning, TEXT("StaticMesh Visibility Set To True_selectionbox construct"));
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

}
