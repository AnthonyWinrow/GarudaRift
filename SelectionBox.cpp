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
DEFINE_LOG_CATEGORY(LogSelectionBox_DestroyMesh);

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
	for (int32 i = 0; i < NumPoints; ++i)
	{
		FVector Location;
		FVector Tangent;
		WallSpline->GetLocationAndTangentAtSplinePoint(i, Location, Tangent, ESplineCoordinateSpace::World);

		if (i == 0)
		{
			SplinePointMesh0->SetWorldLocation(Location);
		}
		else if (i == 1)
		{
			SplinePointMesh1->SetWorldLocation(Location);
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
				bMouseHeld = true;
			}
		}
	}
	else
	{
		TimeMousePressed = 0.0f;
		bMouseHeld = false;
	}

	if (bMouseHeld)
	{
		bIsDragging = true;
	}
	else
	{
		bIsDragging = false;
	}

	if (bMouseHeld != bLastMouseHeld)
	{
		UE_LOG(LogSelectionBox_Tick, Log, TEXT("bMouseHeld: %s"), bMouseHeld ? TEXT("True") : TEXT("False"));
		bLastMouseHeld = bMouseHeld;
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

void ASelectionBox::DestroyMesh()
{
	UE_LOG(LogSelectionBox_DestroyMesh, Warning, TEXT("Entering DestroyMesh"));
}
