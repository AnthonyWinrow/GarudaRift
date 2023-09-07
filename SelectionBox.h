#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "SelectionBox.generated.h"

UCLASS()
class GARUDARIFT_API ASelectionBox : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASelectionBox();

	// Mesh to be spawned
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	// Method to destroy static mesh
	void DestroyMesh();

	// Flag to indicate if a control point is currently selected
	UPROPERTY(BlueprintReadWrite, Category = "Control Point")
	bool bIsControlPointSelected;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Static mesh component
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ControlPointMesh0;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ControlPointMesh1;

	// Function to accept left mouse click data
	void LeftClick(bool bIsPressed);

	UStaticMeshComponent* SelectedControlPoint;
	bool bIsDragging;
	FVector2D StoredInitialClickLocation;
	float TimeSinceLeftMousePressed;
	bool bIsLeftMouseButtonHeld;
	bool bIsLeftMousePressed;
	bool bIsMouseOverControlPointMesh;
	FVector StartingLocation;
	FVector InitialLocationOfControlPointMesh0;
	FVector InitialLocationOfControlPointMesh1;
	FVector LeftPosition;
	FVector RightPosition;
	FVector Center;
	TMap<int32, FString> SplinePointMetadata;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	USplineComponent* WallSpline;

	bool bHasCapturedInitialLocation;
};
