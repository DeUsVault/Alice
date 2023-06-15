// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Alice/Interfaces/Interactable.h"
#include "ElevatorButton.generated.h"

UCLASS()
class ALICE_API AElevatorButton : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AElevatorButton();

protected:
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	void Interact(UPrimitiveComponent* HitComponent);
	virtual void Interact_Implementation(UPrimitiveComponent* HitComponent) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> ButtonMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> ButtonOverlapSphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class AElevator> Elevator;

	int32 ButtonFloorNumber = 0;
};
