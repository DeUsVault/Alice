// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Alice/Interfaces/Interactable.h"
#include "ElevatorFloor.generated.h"

UENUM(BlueprintType)
enum class EElevatorFloorState : uint8 {
	OPENING	UMETA(DisplayName = "Doors Opening"),
	CLOSING	UMETA(DisplayName = "Doors Closing"),
	OPEN	UMETA(DisplayName = "Doors Open"),
	CLOSED	UMETA(DisplayName = "Doors Closed")
};

UCLASS()
class ALICE_API AElevatorFloor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AElevatorFloor();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	void Interact();
	virtual void Interact_Implementation() override;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

public:	

	void OpenDoors();
	void CloseDoors();
	void CallElevator();

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class AElevator> Elevator;

	UPROPERTY(EditAnywhere)
	float DoorMoveSpeed = 1.f;
	FVector LargeDoorOpenPos;
	FVector SmallDoorOpenPos;
	FVector LargeDoorTarget;
	FVector SmallDoorTarget;

	UPROPERTY(VisibleAnywhere)
	EElevatorFloorState FloorState;
	
	UPROPERTY(EditAnywhere)
	int32 FloorNumber;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Frame;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> LargeDoor;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> SmallDoor;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Button;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> ButtonOverlapSphere;

	UPROPERTY(Replicated, VisibleAnywhere)
	bool bCallButtonPressed = false;

// Getters and Setters
public:
	int32 GetFloorNumber() { return FloorNumber; }
	void SetFloorNumber(int32 FloorNum) { FloorNumber = FloorNum; }
	void SetCallButtonPressed(bool pressed);

};
