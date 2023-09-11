#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "SelectionBox.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_Constructor, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_LeftClick, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_DestroyMesh, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_BeginPlay, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_Tick, Log, All);

UCLASS()
class GARUDARIFT_API ASelectionBox : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASelectionBox();

	UPROPERTY()
	FVector TargetLocation;

	void DestroyMesh();
	void LeftClick(bool bIsPressed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ControlPointMesh;

	UPROPERTY(BlueprintReadWrite, Category = "Control Point")
	bool bIsControlPointSelected;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	USplineComponent* WallSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	USplineMeshComponent* WallSplineMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control Point", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ControlPointMesh0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control Point", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ControlPointMesh1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control Point", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SelectedControlPoint;

	UPROPERTY()
	TMap<int32, FString> SplinePointMetadata;

	bool bIsDragging;
	float TimeSinceLeftMousePressed;
	bool bIsLeftMouseButtonHeld;
	bool bIsLeftMousePressed;
	bool bIsMouseOverControlPointMesh;
	bool bHasLoggedTickEntry;
	FVector StartingLocation;
	FVector InitialLocationOfControlPointMesh0;
	FVector InitialLocationOfControlPointMesh1;
	FVector LeftPosition;
	FVector RightPosition;
	FVector Center;
	FVector2D StoredInitialClickLocation;
	UStaticMesh* Wall;

	bool bHasCapturedInitialLocation;
	FVector InitialWallMeshLocation;
};
