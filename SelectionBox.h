#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionBox.generated.h"

UCLASS()
class GARUDARIFT_API ASelectionBox : public AActor
{
	GENERATED_BODY()
	
public:		
	// Sets default values for this actor's properties
	ASelectionBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Static mesh component
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;
};
