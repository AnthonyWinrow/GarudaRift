// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SelectionBox.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_Constructor, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_BeginPlay, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_Tick, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_LeftClick, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_RightClick, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_DestroyMesh, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_WrapMouseAtScreenEdge, Log, All);

UCLASS()
class GARUDARIFT_API ASelectionBox : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's	 properties
	ASelectionBox();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USplineComponent* WallSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USplineMeshComponent* WallVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* StaticMeshParent;

	UPROPERTY(BlueprintReadWrite, Category = "SelectionBox")
	bool bIsSelectionBoxActive;

	UPROPERTY()
	UStaticMeshComponent* SplinePointMesh0;

	UPROPERTY()
	UStaticMeshComponent* SplinePointMesh1;

	UPROPERTY()
	UStaticMesh* Wall;

	void LeftClick(bool bIsPressed);
	void RightClick(bool bRightPressed);
	void DestroyMesh();
	bool bIsLeftMousePressed;
	bool bIsRightMousePressed;
	bool bIsDragging;
	bool bDraggingAnySplinePointMesh;
	bool bIsDraggingSplinePointMesh0;
	bool bIsDraggingSplinePointMesh1;
	bool bMouseHeld;
	bool bLastMouseHeld;
	bool bLastIsDragging;
	bool bClamped;
	bool bDrawDebugShapes;
	float MouseSpeed;
	float TimeMousePressed;
	FVector NewMesh0Location;
	FVector NewMesh1Location;
	FVector Mesh0Location;
	FVector Mesh1Location;
	FVector PreviousDragDirection = FVector::ZeroVector;
	FVector InitialLocationOfSplinePointMesh0;
	FVector InitialLocationOfSplinePointMesh1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool bInitialMousePositionSet;
	FVector InitialMousePosition;
	FVector InitialOffset;
	TArray<FVector> ConstrainedTargetHistory;
	const int HistorySize = 5;
};
