#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h"

ASelectionBox::ASelectionBox()
{
	// Set the actor to call tick every frame
	PrimaryActorTick.bCanEverTick = true;

	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	UE_LOG(LogTemp, Warning, TEXT("ASelectionBox Constructor Called"));

	// Load the custom mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CustomMeshAsset(TEXT("StaticMesh'/Game/GarudaRift/Meshes/Architecture/VFX/SelectionVolume.SelectionVolume'"));
	if (CustomMeshAsset.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Mesh Found"));

		// Get the mesh component from the parent class (ADynamicMeshActor) and set the mesh
		MeshComponent->SetStaticMesh(CustomMeshAsset.Object);
	}
	else
	{
		// Log message for debugging
		UE_LOG(LogTemp, Warning, TEXT("Mesh Not Found"));
	}
}
