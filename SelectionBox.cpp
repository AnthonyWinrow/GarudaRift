#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ASelectionBox::ASelectionBox()
{
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

void ASelectionBox::LeftClick(const FVector& ClickLocation)
{
	// Spawn the mesh at the given location
	FActorSpawnParameters SpawnParams;
	GetWorld()->SpawnActor<ASelectionBox>(ASelectionBox::StaticClass(), ClickLocation, FRotator(0), SpawnParams);

	UE_LOG(LogTemp, Log, TEXT("Mesh Spawned on LeftClick at Location: %s_selectionbox_leftclick"));
}

void ASelectionBox::DestroyMesh()
{
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
