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
}

//Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	UE_LOG(LogSelectionBox_BeginPlay, Warning, TEXT("Entering BeginPlay"));
	
	Super::BeginPlay();

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

				ControlPoint0Location = ControlPointMesh0->GetComponentLocation();
				ControlPoint1Location = ControlPointMesh1->GetComponentLocation();

				NewLocation.Z = CurrentControlPoint0Location.Z;

				WallSpline->SetLocationAtSplinePoint(0, ControlPoint0Location, ESplineCoordinateSpace::World);
				WallSpline->SetLocationAtSplinePoint(1, ControlPoint1Location, ESplineCoordinateSpace::World);
				WallSpline->UpdateSpline();

				FVector SplinePoint0Location = WallSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
				FVector SplinePoint1Location = WallSpline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);

				FVector AutoStartTangent = WallSpline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World).GetSafeNormal();
				FVector AutoEndTangent = WallSpline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World).GetSafeNormal();

				WallSplineMesh->SetStartAndEnd(SplinePoint0Location, AutoStartTangent, SplinePoint1Location, AutoEndTangent);
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
