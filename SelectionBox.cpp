// Fill out your copyright notice in the Description page of Project Settings.

#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h" 
#include "UObject/ConstructorHelpers.h"
#include "Materials/Material.h"

DEFINE_LOG_CATEGORY(LogSelectionBox_Constructor);
DEFINE_LOG_CATEGORY(LogSelectionBox_BeginPlay);
DEFINE_LOG_CATEGORY(LogSelectionBox_Tick);
DEFINE_LOG_CATEGORY(LogSelectionBox_LeftClick);
DEFINE_LOG_CATEGORY(LogSelectionBox_RightClick);
DEFINE_LOG_CATEGORY(LogSelectionBox_DestroyMesh);

ASelectionBox::ASelectionBox()
{
	UE_LOG(LogSelectionBox_Constructor, Warning, TEXT("WallSpline Initialization = Success"));

	PrimaryActorTick.bCanEverTick = true;

	TimeMousePressed = 0.0f;
	bMouseHeld = false;
	bIsDragging = false;
	bLastMouseHeld = false;
	bLastIsDragging = false;
	bInitialMousePositionSet = false;
	bIsDraggingSplinePointMesh0 = false;
	bIsDraggingSplinePointMesh1 = false;
	bClamped = false;
	MouseSpeed = 0.25f;
	LastMousePosition = FVector2D(0, 0);

	WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));
	RootComponent = WallSpline;

	WallVolume = CreateDefaultSubobject<USplineMeshComponent>(TEXT("WallVolume"));
	WallVolume->SetMobility(EComponentMobility::Movable);

	Wall = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/GarudaRift/Meshes/Architecture/VFX/SelectionBox_garudarift.SelectionBox_garudarift")));
	if (Wall)
	{
		WallVolume->SetStaticMesh(Wall);
	}
	else
	{
		UE_LOG(LogSelectionBox_Constructor, Error, TEXT("Failed to Load WallVolume Mesh"));
	}

	UMaterial* WallMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/GarudaRift/Materials/VFX/SelectionVolume.SelectionVolume")));
	if (WallMaterial)
	{
		WallVolume->SetMaterial(0, WallMaterial);
	}
	else
	{
		UE_LOG(LogSelectionBox_Constructor, Error, TEXT("Failed to Load WallVolume Material"));
	}

	StaticMeshParent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshParent"));
	StaticMeshParent->SetupAttachment(RootComponent);

	SplinePointMesh0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SplinePointMesh0"));
	SplinePointMesh0->SetupAttachment(StaticMeshParent);

	SplinePointMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SplinePointMesh1"));
	SplinePointMesh1->SetupAttachment(StaticMeshParent);

	FVector NewScale = FVector(0.2f, 0.2f, 0.2f);
	SplinePointMesh0->SetWorldScale3D(NewScale);
	SplinePointMesh1->SetWorldScale3D(NewScale);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		SplinePointMesh0->SetStaticMesh(SphereMeshAsset.Object);
		SplinePointMesh1->SetStaticMesh(SphereMeshAsset.Object);
	}
	else
	{
		UE_LOG(LogSelectionBox_Constructor, Error, TEXT("SplinePointMesh Initialization = Failed"));
	}

	UMaterial* ControlPointMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/GarudaRift/Materials/VFX/ControlPoint.ControlPoint")));
	if (ControlPointMaterial)
	{
		SplinePointMesh0->SetMaterial(0, ControlPointMaterial);
		SplinePointMesh1->SetMaterial(0, ControlPointMaterial);
	}

	UE_LOG(LogSelectionBox_Constructor, Log, TEXT("Entering Constructor"));
	UE_LOG(LogSelectionBox_Constructor, Log, TEXT("StaticMeshParent Created and Attached"));
	UE_LOG(LogSelectionBox_Constructor, Log, TEXT("SplinePointMesh Initialization = Success"));
}

void ASelectionBox::BeginPlay()
{
	UE_LOG(LogSelectionBox_BeginPlay, Warning, TEXT("Entering BeginPlay"));

	Super::BeginPlay();

	InitializeSplinePoints();
	InitializeSplineTangents();
	InitializeWallVolume();
}

void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMousePositionAndHitResult();
	UpdateCursorType();
	HandleMousePress(DeltaTime);
	DraggableComponents();
	DragLogic();
	UpdateSplineAndVolume();
	CursorVisibility();
}

void ASelectionBox::InitializeSplinePoints()
{
	int32 NumPoints = WallSpline->GetNumberOfSplinePoints();
	Offset.Set(0, 0, 100);
	for (int32 i = 0; i < NumPoints; ++i)
	{
		FVector Location;
		FVector Tangent;
		WallSpline->GetLocationAndTangentAtSplinePoint(i, Location, Tangent, ESplineCoordinateSpace::World);

		if (i == 0)
		{
			Mesh0Location = Location + Offset;
			SplinePointMesh0->SetWorldLocation(Mesh0Location);
			InitialLocationOfSplinePointMesh0 = Mesh0Location;
		}
		else if (i == 1)
		{
			Mesh1Location = Location + Offset;
			SplinePointMesh1->SetWorldLocation(Mesh1Location);
			InitialLocationOfSplinePointMesh1 = Mesh1Location;
		}
	}
}

void ASelectionBox::InitializeSplineTangents()
{
	int32 NumPoints = WallSpline->GetNumberOfSplinePoints();
	for (int32 i = 0; i < NumPoints - 1; ++i)
	{
		FVector StartLocation, StartTangent, EndLocation, EndTangent;
		WallSpline->GetLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent, ESplineCoordinateSpace::World);
		WallSpline->GetLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent, ESplineCoordinateSpace::World);
	}
}

void ASelectionBox::InitializeWallVolume()
{
	if (WallVolume && WallSpline)
	{
		FVector StartLocation, StartTangent, EndLocation, EndTangent;
		WallSpline->GetLocationAndTangentAtSplinePoint(0, StartLocation, StartTangent, ESplineCoordinateSpace::World);
		WallSpline->GetLocationAndTangentAtSplinePoint(1, EndLocation, EndTangent, ESplineCoordinateSpace::World);

		WallVolume->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
		WallVolume->SetupAttachment(WallSpline);
	}
	else
	{
		UE_LOG(LogSelectionBox_BeginPlay, Error, TEXT("WallVolume or WallSpline is Null"));
	}
}

void ASelectionBox::UpdateMousePositionAndHitResult()
{
	PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);

	HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);
}

void ASelectionBox::UpdateCursorType()
{
	EMouseCursor::Type NewCursor = EMouseCursor::Default;

	if (HitResult.IsValidBlockingHit())
	{
		if (HitResult.GetActor() == this)
		{
			if (HitResult.GetComponent() == SplinePointMesh0 || HitResult.GetComponent() == SplinePointMesh1 || HitResult.GetComponent() == BendPointMesh)
			{
				NewCursor = EMouseCursor::Hand;
			}
		}
	}

	PlayerController->CurrentMouseCursor = NewCursor;
}

void ASelectionBox::HandleMousePress(float DeltaTime)
{
	if (bIsLeftMousePressed)
	{
		TimeMousePressed += DeltaTime;
		if (TimeMousePressed >= 0.2f)
		{
			if (HitResult.IsValidBlockingHit())
			{
				if (HitResult.GetComponent() == SplinePointMesh0 || HitResult.GetComponent() == SplinePointMesh1)
				{
					bIsDragging = true;
				}
			}
		}		
	}
	else
	{
		TimeMousePressed = 0.0f;
		bIsDragging = false;
	}

	if (bIsRightMousePressed)
	{
		FVector2D CurrentMousePosition;
		PlayerController->GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);

		if (LastMousePosition != CurrentMousePosition)
		{
			int32 ScreenWidth, ScreenHeight;
			PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);
			FVector2D CenterOfScreen = FVector2D(ScreenWidth, ScreenHeight) / 2.0f;
			PlayerController->SetMouseLocation(FMath::FloorToInt(CenterOfScreen.X), FMath::FloorToInt(CenterOfScreen.Y));
		}

		LastMousePosition = CurrentMousePosition;
	}

}

void ASelectionBox::DraggableComponents()
{
	if (PlayerController)
	{
		if (bIsDragging && HitResult.IsValidBlockingHit())
		{
			if (HitResult.GetComponent() == SplinePointMesh0)
			{
				bIsDraggingSplinePointMesh0 = true;
				bIsDraggingSplinePointMesh1 = false;
				bDraggingEndPointMesh = true;
			}
			else if (HitResult.GetComponent() == SplinePointMesh1)
			{
				bIsDraggingSplinePointMesh1 = true;
				bIsDraggingSplinePointMesh0 = false;
				bDraggingEndPointMesh = true;
			}
		}

		if (!bIsDragging)
		{
			bIsDraggingSplinePointMesh0 = false;
			bIsDraggingSplinePointMesh1 = false;
			bDraggingEndPointMesh = false;
		}
	}
}

void ASelectionBox::DragLogic()
{
	static FVector LastLocation;
	LastMousePosition;
	static bool bFirstDrag = true;
	FVector CurrentLocation;
	FVector HitLocation;

	if (bIsDragging && (bIsDraggingSplinePointMesh0 || bIsDraggingSplinePointMesh1))
	{
		USceneComponent* TargetMesh = bIsDraggingSplinePointMesh0 ? SplinePointMesh0 : SplinePointMesh1;
		FVector InitialLocation = bIsDraggingSplinePointMesh0 ? InitialLocationOfSplinePointMesh0 : InitialLocationOfSplinePointMesh1;

		CurrentLocation = TargetMesh->GetComponentLocation();
		HitLocation = HitResult.Location;

		if (FVector::DistSquared(HitLocation, LastLocation) > FMath::Square(1.0f))
		{
			FVector DragDistance = HitLocation - CurrentLocation;
			FVector WallSplineForwardVector = WallSpline->GetForwardVector().GetSafeNormal();
			FVector PlanePoint = CurrentLocation;
			FVector PlaneNormal = FVector(0, 0, 1);
			FVector ProjectedHitLocation = HitLocation - ((HitLocation - PlanePoint) | PlaneNormal) * PlaneNormal;
			FVector DesiredMovement = HitLocation - CurrentLocation;
			FVector ConstrainedMovement = FVector::DotProduct(DesiredMovement, WallSplineForwardVector) * WallSplineForwardVector;
			ConstrainedMovement *= MouseSpeed;			);

			if (bIsDraggingSplinePointMesh0)
			{
				if (ConstrainedMovement.X > 0 && CurrentLocation.X >= InitialLocation.X)
				{
					ConstrainedMovement.X = 0;
				}
				if (ConstrainedMovement.Y > 0 && CurrentLocation.Y >= InitialLocation.Y)
				{
					ConstrainedMovement.Y = 0;
				}
				if (ConstrainedMovement.Z > 0 && CurrentLocation.Z >= InitialLocation.Z)
				{
					ConstrainedMovement.Z = 0;
				}
			}
			else if (bIsDraggingSplinePointMesh1)
			{
				if (ConstrainedMovement.X < 0 && CurrentLocation.X <= InitialLocation.X)
				{
					ConstrainedMovement.X = 0;
				}
				if (ConstrainedMovement.Y < 0 && CurrentLocation.Y <= InitialLocation.Y)
				{
					ConstrainedMovement.Y = 0;
				}
				if (ConstrainedMovement.Z < 0 && CurrentLocation.Z <= InitialLocation.Z)
				{
					ConstrainedMovement.Z = 0;
				}
			}

			FVector NewLocation = CurrentLocation + ConstrainedMovement;

			if (bIsDraggingSplinePointMesh0)
			{
				if (NewLocation.X > InitialLocation.X)
				{
					NewLocation.X = InitialLocation.X;
				}
				if (NewLocation.Y > InitialLocation.Y)
				{
					NewLocation.Y = InitialLocation.Y;
				}
				if (NewLocation.Z > InitialLocation.Z)
				{
					NewLocation.Z = InitialLocation.Z;
				}
			}
			else if (bIsDraggingSplinePointMesh1)
			{
				if (NewLocation.X < InitialLocation.X)
				{
					NewLocation.X = InitialLocation.X;
				}
				if (NewLocation.Y < InitialLocation.Y)
				{
					NewLocation.Y = InitialLocation.Y;
				}
				if (NewLocation.Z < InitialLocation.Z)
				{
					NewLocation.Z = InitialLocation.Z;
				}
			}

			TargetMesh->SetWorldLocation(NewLocation);
			LastLocation = HitLocation;

			if (NewLocation.Equals(InitialLocation, 1.0f))
			{
				FVector2D ScreenLocation;
				PlayerController->ProjectWorldLocationToScreen(InitialLocation, ScreenLocation);
				PlayerController->SetMouseLocation(FMath::FloorToInt(ScreenLocation.X), FMath::FloorToInt(ScreenLocation.Y));
			}

			UE_LOG(LogSelectionBox_Tick, Log, TEXT("NewLocation =: %s"), *NewLocation.ToString());

			if (TargetMesh == SplinePointMesh0)
			{
				FVector CurrentLocation0 = WallSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
				FVector UpdatedLocation0 = FVector(NewLocation.X, NewLocation.Y, CurrentLocation0.Z);
				WallSpline->SetLocationAtSplinePoint(0, UpdatedLocation0, ESplineCoordinateSpace::World);
			}
			else if (TargetMesh == SplinePointMesh1)
			{
				FVector CurrentLocation1 = WallSpline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);
				FVector UpdatedLocation1 = FVector(NewLocation.X, NewLocation.Y, CurrentLocation1.Z);
				WallSpline->SetLocationAtSplinePoint(1, UpdatedLocation1, ESplineCoordinateSpace::World);
			}
		}
	}
}

void ASelectionBox::UpdateSplineAndVolume()
{
	if (WallSpline && WallVolume)
	{
		FVector LocalStartPoint = WallSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector LocalEndPoint = WallSpline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);
		FVector StartTangent = WallSpline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector EndTangent = WallSpline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);

		WallVolume->SetStartAndEnd(LocalStartPoint, StartTangent, LocalEndPoint, EndTangent);
	}
}

void ASelectionBox::CursorVisibility()
{
	if (bIsDragging || bIsRightMousePressed)
	{
		PlayerController->bShowMouseCursor = false;
	}
	else if (!bIsDragging || !bIsRightMousePressed)
	{
		PlayerController->bShowMouseCursor = true;
	}
}

void ASelectionBox::LeftClick(bool bIsPressed)
{
	UE_LOG(LogSelectionBox_LeftClick, Warning, TEXT("Entering LeftClick"));

	if (bIsPressed)
	{
		bIsLeftMousePressed = true;
	}
	else
	{
		bIsLeftMousePressed = false;
	}

	UE_LOG(LogSelectionBox_LeftClick, Log, TEXT("bIsLeftMousePressed: %s"), bIsLeftMousePressed ? TEXT("True") : TEXT("False"));
}

void ASelectionBox::RightClick(bool bRightPressed)
{
	UE_LOG(LogSelectionBox_RightClick, Warning, TEXT("Entering RightClick"));

	if (bRightPressed)
	{
		bIsRightMousePressed = true;
	}
	else
	{
		bIsRightMousePressed = false;
	}

	UE_LOG(LogSelectionBox_RightClick, Log, TEXT("bIsRightMousePressed=: %s"), bRightPressed ? TEXT("True") : TEXT("False"));
}

void ASelectionBox::DestroyMesh()
{
	UE_LOG(LogSelectionBox_DestroyMesh, Warning, TEXT("Entering DestroyMesh"));

	if (WallVolume)
	{
		WallVolume->DestroyComponent();
		WallVolume = nullptr;
	}

	if (SplinePointMesh0)
	{
		SplinePointMesh0->DestroyComponent();
		SplinePointMesh0 = nullptr;
	}

	if (SplinePointMesh1)
	{
		SplinePointMesh1->DestroyComponent();
		SplinePointMesh1 = nullptr;
	}
}
