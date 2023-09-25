// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "SelectionBox.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_Constructor, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_BeginPlay, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_Tick, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_LeftClick, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSelectionBox_DestroyMesh, Log, All);

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
	UStaticMeshComponent* StaticMeshParent;

	UPROPERTY()
	UStaticMeshComponent* SplinePointMesh0;

	UPROPERTY()
	UStaticMeshComponent* SplinePointMesh1;

	void LeftClick(bool bIsPressed);
	void DestroyMesh();

	bool bIsLeftMousePressed;
	bool bIsDragging;
	bool bMouseHeld;
	bool bLastMouseHeld;
	bool bLastIsDragging;
	float TimeMousePressed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
