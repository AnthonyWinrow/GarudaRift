#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "E:/EpicGames/UE_5.2/Engine/Source/Runtime/Engine/Classes/Engine/StaticMesh.h"

ASelectionBox::ASelectionBox()
{
	// Set the actor to call tick every frame
	PrimaryActorTick.bCanEverTick = true;
	
	// Create a static mesh component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetMobility(EComponentMobility::Movable);
	SetRootComponent(StaticMeshComponent);

	// Load a static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/GarudaRift/Meshes/Architecture/VFX/SelectionVolume.SelectionVolume'"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	}
	else
	{
		// Log message for debugging
		UE_LOG(LogTemp, Warning, TEXT("Mesh Not Found"));
	}
}
