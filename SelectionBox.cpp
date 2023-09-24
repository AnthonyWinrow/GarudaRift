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

	WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));
	RootComponent = WallSpline;
	UE_LOG(LogSelectionBox_Constructor, Log, TEXT("WallSpline Initialized and Set as RootComponent"));

	StaticMeshParent = CreateDefaultSubobject<USceneComponent>(TEXT("StaticMeshParent"));
	UE_LOG(LogSelectionBox_Constructor, Log, TEXT("StaticMeshParent Initialized"));

	WallSplineMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("WallSplineMesh"));
	WallSplineMesh->SetupAttachment(RootComponent);
	WallSplineMesh->SetMobility(EComponentMobility::Movable);
	UE_LOG(LogSelectionBox_Constructor, Log, TEXT("WallSplineMesh Initialized"));
	
	Wall = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/GarudaRift/Meshes/Architecture/VFX/SelectionBox_garudarift.SelectionBox_garudarift")));
	if (Wall)
	{
		WallSplineMesh->SetStaticMesh(Wall);
		UE_LOG(LogSelectionBox_Constructor, Log, TEXT("Wall Added and Set"));
	}
	else
	{
		UE_LOG(LogSelectionBox_Constructor, Error, TEXT("Failed to Load Wall"));
	}

	ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset0(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (SphereMeshAsset0.Succeeded())
	{
		ControlPointMeshAsset0 = SphereMeshAsset0.Object;
		UE_LOG(LogSelectionBox_Constructor, Log, TEXT("ControlPointMesh0 Loaded"));
	}
	else
	{
		UE_LOG(LogSelectionBox_Constructor, Error, TEXT("Faled to Load ControlPointMesh0"));
	}

	ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset1(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (SphereMeshAsset1.Succeeded())
	{
		ControlPointMeshAsset1 = SphereMeshAsset1.Object;
		UE_LOG(LogSelectionBox_Constructor, Log, TEXT("ControlPointMesh1 Loaded"));
	}
	else
	{
		UE_LOG(LogSelectionBox_Constructor, Error, TEXT("Faled to Load ControlPointMesh1"));
	}
}

//Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	UE_LOG(LogSelectionBox_BeginPlay, Warning, TEXT("Entering BeginPlay"));
	
	Super::BeginPlay();

	ControlPointMesh0 = NewObject<UStaticMeshComponent>(this);
	ControlPointMesh0->RegisterComponent();
	ControlPointMesh0->AttachToComponent(StaticMeshParent, FAttachmentTransformRules::KeepRelativeTransform);
	ControlPointMesh0->SetStaticMesh(ControlPointMeshAsset0);
	UE_LOG(LogSelectionBox_BeginPlay, Log, TEXT("ControlPointMesh0 Initialized"));

	ControlPointMesh1 = NewObject<UStaticMeshComponent>(this);
	ControlPointMesh1->RegisterComponent();
	ControlPointMesh1->AttachToComponent(StaticMeshParent, FAttachmentTransformRules::KeepRelativeTransform);
	ControlPointMesh1->SetStaticMesh(ControlPointMeshAsset1);
	UE_LOG(LogSelectionBox_BeginPlay, Log, TEXT("ControlPointMesh1 Initialized"));

	FVector Scale = FVector(0.15f, 0.15f, 0.15f);
	ControlPointMesh0->SetWorldScale3D(Scale);
	ControlPointMesh1->SetWorldScale3D(Scale);

	FVector MeshDimensions = WallSplineMesh->GetStaticMesh()->GetBoundingBox().GetExtent();
	LeftEdge = WallSplineMesh->GetComponentLocation() - MeshDimensions / 2;

	ControlPoint0Location = LeftEdge + MeshDimensions / 2;


	FVector Offset = FVector(25, 0, 0);
	ControlPoint1Location = ControlPoint0Location + Offset;

	FVector WallMeshDimensions = WallSplineMesh->GetStaticMesh()->GetBoundingBox().GetSize();
	
	FVector RightVector = WallSplineMesh->GetRightVector();
	FVector MidPoint = -0.5 * WallMeshDimensions * RightVector;
	ControlPoint0Location += MidPoint;
	ControlPoint1Location += MidPoint;

	FVector ScaledRightVector = WallSplineMesh->GetRightVector();
	float ScaleFactor = 0.5f;
	FVector NewScale = WallSplineMesh->GetComponentScale();
	NewScale += (RightVector * ScaleFactor - ScaledRightVector) * NewScale;

	WallSplineMesh->SetWorldScale3D(NewScale);

	InitialWallLocation = WallSplineMesh->GetComponentLocation();

	ControlPointMesh0->SetWorldLocation(ControlPoint0Location);
	ControlPointMesh1->SetWorldLocation(ControlPoint1Location);

	InitialLocationOfControlPointMesh0 = ControlPoint0Location;
	InitialLocationOfControlPointMesh1 = ControlPoint1Location;

	InitialLocationOfControlPoint0 = ControlPoint0Location;
	InitialLocationOfControlPoint1 = ControlPoint1Location;

	InitialTangent0 = WallSpline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);
	InitialTangent1 = WallSpline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);

	WallSpline->SetLocationAtSplinePoint(0, ControlPoint0Location, ESplineCoordinateSpace::World, true);
	WallSpline->SetLocationAtSplinePoint(1, ControlPoint1Location, ESplineCoordinateSpace::World, true);
	UE_LOG(LogSelectionBox_BeginPlay, Log, TEXT("ControlPointMesh0 Location: %s, ControlPointMesh1 Location: %s"), *LeftEdge.ToString(), *RightEdge.ToString());

	StartTangent = FVector(1, 0, 0) * 200.0f;
	EndTangent = FVector(1, 0, 0) * 200.0f;

	StartPos = WallSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	EndPos = WallSpline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);

	WallSplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
}  

// Called every frame
void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsLeftMousePressed)
	{
		UE_LOG(LogSelectionBox_Tick, Log, TEXT("bIsLeftMousePressed = True"));
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

				FVector WallForward = WallSplineMesh->GetForwardVector();
				TargetLocation = FVector::DotProduct(HitResult.Location - WallSplineMesh->GetComponentLocation(), WallForward) * WallForward + WallSplineMesh->GetComponentLocation();

				FVector CurrentLocation = ControlPointMesh0->GetComponentLocation();
				FVector NewLocation = FMath::Lerp(CurrentLocation, TargetLocation, 0.03f);

				FVector CurrentControlPoint0Location = WallSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

				NewLocation.Z = CurrentLocation.Z;

				float InitialProjection = FVector::DotProduct(InitialLocationOfControlPointMesh0, WallForward);
				float NewProjection = FVector::DotProduct(NewLocation, WallForward);
				float Buffer = 0.1f;

				if (NewProjection > InitialProjection + Buffer)
				{
					NewLocation = InitialLocationOfControlPointMesh0;

					FVector2D ScreenMousePosition;
					PlayerController->GetMousePosition(ScreenMousePosition.X, ScreenMousePosition.Y);
					FVector2D ScreenLocation;
					PlayerController->ProjectWorldLocationToScreen(NewLocation, ScreenLocation);
					PlayerController->SetMouseLocation(ScreenLocation.X, ScreenLocation.Y);
				}

				ControlPointMesh0->SetWorldLocation(NewLocation);
				UE_LOG(LogSelectionBox_Tick, Warning, TEXT("ControlPoint0Location: %s"), *ControlPoint0Location.ToString());

				ControlPoint0Location = ControlPointMesh0->GetComponentLocation();

				FVector UpdateTangent = ControlPoint0Location - WallSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
				WallSpline->SetTangentAtSplinePoint(0, UpdateTangent, ESplineCoordinateSpace::World, true);
				WallSpline->SetLocationAtSplinePoint(0, ControlPoint0Location, ESplineCoordinateSpace::World, true);

				FVector ActualSplinePoint = WallSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
				FVector NextSplinePoint = WallSpline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);

				UE_LOG(LogSelectionBox_Tick, Warning, TEXT("Is Spline Mesh Updating?"));

				FVector Tangent = NewLocation - ControlPoint0Location;
				Tangent.Normalize();

				FVector DebugStartBefore = WallSplineMesh->GetStartPosition();
				FVector DebugEndBefore = WallSplineMesh->GetEndPosition();
				UE_LOG(LogSelectionBox_Tick, Warning, TEXT("DebugStartBefore: %s"), *DebugStartBefore.ToString());
				UE_LOG(LogSelectionBox_Tick, Warning, TEXT("DebusEndBefore: %s"), *DebugEndBefore.ToString());

				WallSplineMesh->SetStartAndEnd(ControlPoint0Location, FVector::ZeroVector, NewLocation, FVector::ZeroVector, true);

				FVector DebugTangent = WallSpline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);

				FVector DebugStartAfter = WallSplineMesh->GetStartPosition();
				FVector DebugEndAfter = WallSplineMesh->GetEndPosition();
				UE_LOG(LogSelectionBox_Tick, Warning, TEXT("DebugStartAfter: %s"), *DebugStartAfter.ToString());
				UE_LOG(LogSelectionBox_Tick, Warning, TEXT("DebugEndAfter: %s"), *DebugEndAfter.ToString());
				FVector DebugSplinePoint = WallSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
				UE_LOG(LogSelectionBox_Tick, Warning, TEXT("DebugSplinePoint: %s"), *DebugSplinePoint.ToString());
				UE_LOG(LogSelectionBox_Tick, Warning, TEXT("DebugTangent: %s"), *DebugTangent.ToString());

				WallSplineMesh->SetStartTangent(DebugTangent, true);
				WallSplineMesh->SetEndTangent(DebugTangent, true);
				WallSplineMesh->MarkRenderStateDirty();
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
