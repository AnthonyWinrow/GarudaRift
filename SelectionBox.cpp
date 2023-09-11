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

	PrimaryActorTick.bCanEverTick = true;
	bHasLoggedTickEntry = false;
	TimeSinceLeftMousePressed = 0.0f;
	bIsLeftMouseButtonHeld = false;
	bIsLeftMousePressed = false;
	bIsDragging = false;

	WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));
	RootComponent = WallSpline;

	SplinePointMetadata.Add(0, "ControlPoint0");
	FString* MetaDataValue = SplinePointMetadata.Find(0);
	if (MetaDataValue)
	{
		const TCHAR* MetaDataChar = **MetaDataValue;
		UE_LOG(LogSelectionBox_Constructor, Log, TEXT("MetaData fo ControlPoint0 Added Successfully: %s"), MetaDataChar);
	}
	else
	{
		UE_LOG(LogSelectionBox_Constructor, Error, TEXT("Faled to Add MetaData for ControlPoint0"));
	}

	WallSplineMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("WallSplineMesh"));
	WallSplineMesh->SetMobility(EComponentMobility::Movable);
	WallSplineMesh->SetupAttachment(RootComponent);

	ControlPointMesh0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControlPointMesh0"));
	ControlPointMesh0->SetupAttachment(WallSpline);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (SphereMeshAsset.Succeeded())
	{
		ControlPointMesh0->SetStaticMesh(SphereMeshAsset.Object);
		UE_LOG(LogSelectionBox_BeginPlay, Log, TEXT("ControlPointMesh0 Initialized and Mesh Attached"));
	}

	FVector LocalForwardVector = WallSplineMesh->GetForwardVector();
	FVector LocalLeftOffset = -LocalForwardVector * 5.0f;

	ControlPointMesh0->SetRelativeLocation(LocalLeftOffset);
	UE_LOG(LogSelectionBox, Log, TEXT("ControlPointMesh0 Local Left Offset Set to: %s"), *LocalLeftOffset.ToString());

	FVector Scale(0.13f, 0.13f, 0.13f);
	ControlPointMesh0->SetWorldScale3D(Scale);
	UE_LOG(LogSelectionBox_Constructor, Log, TEXT("ControlPointMesh0 Scale to: %s"), *Scale.ToString());
}

//Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	UE_LOG(LogSelectionBox_BeginPlay, Warning, TEXT("Entering BeginPlay"));
	
	Super::BeginPlay();

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
	FVector WallScale = FVector(1.0f, 0.4f, 1.0f);
	WallSplineMesh->SetRelativeScale3D(WallScale);
	UE_LOG(LogSelectionBox_BeginPlay, Log, TEXT("WallSplineMesh Scale Set to: %s"), *WallScale.ToString());

	FVector ZOffset = FVector(0, 0, 150.0f);
	ControlPointMesh0->AddRelativeLocation(ZOffset);;
	UE_LOG(LogSelectionBox_BeginPlay, Log, TEXT("ControlPointMesh0 Z-Offset Set to: %s"), *ZOffset.ToString());

	if (ControlPointMesh0)
	{
		InitialLocationOfControlPointMesh0 = ControlPointMesh0->GetComponentLocation();
		UE_LOG(LogSelectionBox_BeginPlay, Log, TEXT("Initial Location of ControlPointMesh0 Stored: %s"), *InitialLocationOfControlPointMesh0.ToString());
	}
	else
	{
		UE_LOG(LogSelectionBox, Error, TEXT("ControlPointMesh0 Is Not Initialized"));
	}
}

// Called every frame
void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsLeftMousePressed)
	{
		UE_LOG(LogSelectionBox_Tick, Log, TEXT("bIsLefMousePressed = True"));
		TimeSinceLeftMousePressed += DeltaTime;
		if (TimeSinceLeftMousePressed >= 0.2f && !bIsLeftMouseButtonHeld)
		{
			bIsLeftMouseButtonHeld = true;
			bIsDragging = true;
			UE_LOG(LogSelectionBox_Tick, Warning, TEXT("Left Click Holding"));
		}
	}
	else
	{
		TimeSinceLeftMousePressed = 0.0f;
		bIsLeftMouseButtonHeld = false;
		bIsDragging = false;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		FVector2D MousePosition;
		if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
		{
			FHitResult HitResult;
			PlayerController->GetHitResultAtScreenPosition(MousePosition, ECC_Visibility, true, HitResult);

			AActor* HitActor = HitResult.GetActor();
			UPrimitiveComponent* HitComponent = HitResult.GetComponent();
			
			if (HitActor == this && HitComponent == ControlPointMesh0)
			{
				PlayerController->CurrentMouseCursor = EMouseCursor::Hand;
			}
			else
			{
				PlayerController->CurrentMouseCursor = EMouseCursor::Default;
			}

			if (bIsDragging)
			{
				PlayerController->bShowMouseCursor = false;
				FVector NewLocation = HitResult.Location;

			}
			else
			{
				PlayerController->bShowMouseCursor = true;
			}

		}
	}
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

	if (WallSplineMesh)
	{

		WallSplineMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		WallSplineMesh->DestroyComponent();
		WallSplineMesh = nullptr;
		UE_LOG(LogSelectionBox_DestroyMesh, Log, TEXT("SplineMesh Destruction = Success"));
	}
	else
	{
		UE_LOG(LogSelectionBox_DestroyMesh, Error, TEXT("WallSplineMesh is NULL in DestroyMesh"));
	}

	if (ControlPointMesh0)
	{
		ControlPointMesh0->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		ControlPointMesh0->DestroyComponent();
		ControlPointMesh0 = nullptr;
		UE_LOG(LogSelectionBox_DestroyMesh, Log, TEXT("ControlPointMesh0 Destruction = Success"));
	}
	else
	{
		UE_LOG(LogSelectionBox, Error, TEXT("ControlPointMesh0 is NULL"));
	}

	Destroy();
	UE_LOG(LogSelectionBox_DestroyMesh, Log, TEXT("SelectionBox Actor Destruction = Success"));
}
