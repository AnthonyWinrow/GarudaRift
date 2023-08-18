#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/GarudaRift/Meshes/Architecture/VFX/SelectionVolume.SelectionVolume")); 
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

