#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/ConstructorHelpers.h"
#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/CharacterLogic/BuildMode.h"
#include "Misc/OutputDeviceDebug.h"
#include "Components/SplineMeshComponent.h"

DEFINE_LOG_CATEGORY(LogSelectionBox);

DEFINE_LOG_CATEGORY(LogSelectionBox_Constructor);
DEFINE_LOG_CATEGORY(LogSelectionBox_LeftClick);
DEFINE_LOG_CATEGORY(LogSelectionBox_DestroyMesh);
DEFINE_LOG_CATEGORY(LogSelectionBox_BeginPlay);
DEFINE_LOG_CATEGORY(LogSelectionBox_Tick);

//Sets default values
ASelectionBox::ASelectionBox()
{
	UE_LOG(LogSelectionBox_Constructor, Warning, TEXT("Entering Constructor"));

	WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));
	RootComponent = WallSpline;

	WallSplineMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("WallSplineMesh"));
	WallSplineMesh->SetMobility(EComponentMobility::Movable);
	WallSplineMesh->SetupAttachment(RootComponent);
}

//Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	UE_LOG(LogSelectionBox_BeginPlay, Warning, TEXT("Entering BeginPlay"));

	Wall = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/GarudaRift/Meshes/Architecture/VFX/SelectionBox_garudarift.SelectionBox_garudarift'")));
	if (Wall)
	{
		WallSplineMesh->SetStaticMesh(Wall);
		UE_LOG(LogSelectionBox_BeginPlay, Log, TEXT("Successfully Set the Static Mesh for WallSplineMesh"));
	}
	else
	{
		UE_LOG(LogSelectionBox_BeginPlay, Error, TEXT("Failed to Set the Static Mesh for WallSplineMesh"));
	}
}

// Called every frame
void ASelectionBox::Tick(float DeltaTime)
{

}

void ASelectionBox::LeftClick(bool bIsPressed)
{
	// Set the internal state of left click
	bIsLeftMousePressed = bIsPressed;

	// Logging: State of left bIsLeftMousePressed
	UE_LOG(LogSelectionBox_LeftClick, Log, TEXT("LeftClick Called: %s"), bIsLeftMousePressed ? TEXT("True") : TEXT("False"));
}

void ASelectionBox::DestroyMesh()
{
	UE_LOG(LogSelectionBox_DestroyMesh, Warning, TEXT("Initiating DestroyMesh"));
	if (!IsValid(this))
	{
		UE_LOG(LogSelectionBox, Error, TEXT("SelectionBox Actor is Not Valid or Already Destroyed"));
		return;
	}

	if (!WallSplineMesh)
	{
		UE_LOG(LogSelectionBox_DestroyMesh, Error, TEXT("WallSplineMesh is NULL in DestroyMesh"));
	}

	WallSplineMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	WallSplineMesh->DestroyComponent();
	WallSplineMesh = nullptr;

	UE_LOG(LogSelectionBox_DestroyMesh, Log, TEXT("SplineMesh Destruction = Success"));

	Destroy();

	UE_LOG(LogSelectionBox_DestroyMesh, Log, TEXT("SelectionBox Actor Destruction = Success"));
}
