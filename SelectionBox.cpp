// Fill out your copyright notice in the Description page of Project Settings.

#include "E:/Unreal Projects/GarudaRift/Source/GarudaRift/ActorClasses/SelectionBox.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY(LogSelectionBox_Constructor);
DEFINE_LOG_CATEGORY(LogSelectionBox_BeginPlay);
DEFINE_LOG_CATEGORY(LogSelectionBox_Tick);
DEFINE_LOG_CATEGORY(LogSelectionBox_LeftClick);
DEFINE_LOG_CATEGORY(LogSelectionBox_RightClick);
DEFINE_LOG_CATEGORY(LogSelectionBox_DestroyMesh);
DEFINE_LOG_CATEGORY(LogSelectionBox_WrapMouseAtScreenEdge);

// Sets default values
ASelectionBox::ASelectionBox()
{
	UE_LOG(LogSelectionBox_Constructor, Warning, TEXT("WallSpline Initialization = Success"));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TimeMousePressed = 0.0f;
	bMouseHeld = false;
	bIsDragging = false;
	bLastMouseHeld = false;
	bLastIsDragging = false;
	bInitialMousePositionSet = false;
	bIsDraggingSplinePointMesh = false;

	WallSpline = CreateDefaultSubobject<USplineComponent>(TEXT("WallSpline"));
	RootComponent = WallSpline;

	StaticMeshParent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshParent"));
	StaticMeshParent->SetupAttachment(RootComponent);

	SplinePointMesh0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SplinePointMesh0"));
	SplinePointMesh0->SetupAttachment(StaticMeshParent);

	SplinePointMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SplinePointMesh1"));
	SplinePointMesh1->SetupAttachment(StaticMeshParent);

	FVector NewScale = FVector(0.3f, 0.3f, 0.3f);
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

	UE_LOG(LogSelectionBox_Constructor, Log, TEXT("Entering Constructor"));
	UE_LOG(LogSelectionBox_Constructor, Log, TEXT("StaticMeshParent Created and Attached"));
	UE_LOG(LogSelectionBox_Constructor, Log, TEXT("SplinePointMesh Initialization = Success"));
}

// Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	UE_LOG(LogSelectionBox_BeginPlay, Warning, TEXT("Entering BeginPlay"));

	Super::BeginPlay();

	int32 NumPoints = WallSpline->GetNumberOfSplinePoints();
	FVector Offset(0, 0, 50);
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
		}

		DrawDebugSphere(GetWorld(), Location, 10.0f, 12, FColor::Blue, true, -1, 0, 1);

		FVector ControlPoint = Location + Tangent;
		DrawDebugBox(GetWorld(), ControlPoint, FVector(5.0f, 5.0f, 5.0f), FColor::White, true, -1, 0, 1);
	}

	for (int32 i = 0; i < NumPoints - 1; ++i)
	{
		FVector StartLocation, StartTangent, EndLocation, EndTangent;
		WallSpline->GetLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent, ESplineCoordinateSpace::World);
		WallSpline->GetLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent, ESplineCoordinateSpace::World);

		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, true, -1, 0, 1);
	}
}

// Called every frame
void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	EMouseCursor::Type NewCursor = EMouseCursor::Default;

	if (HitResult.IsValidBlockingHit())
	{
		if (HitResult.GetActor() == this)
		{
			if (HitResult.GetComponent() == SplinePointMesh0 || HitResult.GetComponent() == SplinePointMesh1)
			{
				NewCursor = EMouseCursor::Hand;
			}
		}
	}

	PlayerController->CurrentMouseCursor = NewCursor;

	if (bIsLeftMousePressed)
	{
		if (HitResult.IsValidBlockingHit() && (HitResult.GetComponent() == SplinePointMesh0 || HitResult.GetComponent() == SplinePointMesh1))
		{
			TimeMousePressed += DeltaTime;
			if (TimeMousePressed >= 0.5f)
			{
				bIsDragging = true;
			}
		}
	}
	else
	{
		TimeMousePressed = 0.0f;
		bIsDragging = false;
	}

	static FVector TargetLocation;
	static FVector LastLocation;
	static FVector SmoothedTargetLocation;
	static bool bFirstDrag = true;
	FVector CurrentLocation;
	FVector HitLocation;

	if (PlayerController)
	{
		if (bIsDragging && HitResult.IsValidBlockingHit() && HitResult.GetComponent() == SplinePointMesh0)
		{
			bIsDraggingSplinePointMesh = true;
		}

		if (!bIsDragging)
		{
			bIsDraggingSplinePointMesh = false;
		}

		if (bIsDragging && bIsDraggingSplinePointMesh)
		{
			CurrentLocation = SplinePointMesh0->GetComponentLocation();
			HitLocation = HitResult.Location;

			if (FVector::DistSquared(HitLocation, LastLocation) > FMath::Square(1.0f))
			{
				if (bFirstDrag)
				{
					TargetLocation = CurrentLocation;
					SmoothedTargetLocation = TargetLocation;
					bFirstDrag = false;
				}

				FVector WallSplineForwardVector = WallSpline->GetForwardVector();
				FVector PlanePoint = CurrentLocation;
				FVector PlaneNormal = FVector(0, 0, 1);
				FVector ProjectedHitLocation = HitLocation - ((HitLocation - PlanePoint) | PlaneNormal) * PlaneNormal;
				FVector DirectionToHit = (HitLocation - CurrentLocation).GetSafeNormal();
				float DotProduct = FVector::DotProduct(DirectionToHit, WallSplineForwardVector);
				FVector ConstrainedTarget = CurrentLocation + WallSplineForwardVector * DotProduct * 100.0f;
				float DistanceToCursor = FVector::Dist(CurrentLocation, ProjectedHitLocation);
				float DynamicAlpha = FMath::Clamp(DistanceToCursor / 100.0f, 1.0f, 4.0f);
				FVector NewLocation = FMath::VInterpTo(CurrentLocation, ConstrainedTarget, DeltaTime, DynamicAlpha);

				if (FVector::DistSquared(NewLocation, LastLocation) > FMath::Square(1.0f))
				{
					SplinePointMesh0->SetWorldLocation(NewLocation);
				}

				LastLocation = HitLocation;
			}

			PlayerController->bShowMouseCursor = false;
			PlayerController->SetShowMouseCursor(false);

			if (!bIsDragging)
			{
				FVector SnapLocation = FMath::VInterpTo(CurrentLocation, HitLocation, DeltaTime, 10.0f);
				SplinePointMesh0->SetWorldLocation(SnapLocation);
			}
		}
	}
	else
	{
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetShowMouseCursor(true);
	}
	
	if (bIsDragging != bLastIsDragging)
	{
		UE_LOG(LogSelectionBox_Tick, Log, TEXT("bIsDragging: %s"), bIsDragging ? TEXT("True") : TEXT("False"));
		bLastIsDragging = bIsDragging;
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
}
