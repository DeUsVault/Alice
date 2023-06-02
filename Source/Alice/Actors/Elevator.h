// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Elevator.generated.h"

UENUM(BlueprintType)
enum class EElevatorState : uint8 {
	IDLE			UMETA(DisplayName = "Idle"),
	GOING_UP		UMETA(DisplayName = "Going Up"),
	GOING_DOWN		UMETA(DisplayName = "Going Down"),
	DOORS_OPENING	UMETA(DisplayName = "Doors Opening"),
	DOORS_CLOSING	UMETA(DisplayName = "Doors Closing"),
	WAITING			UMETA(DisplayName = "Waiting For Occupants")
};

UCLASS()
class ALICE_API AElevator : public AActor
{
	GENERATED_BODY()
	
public:	
	AElevator();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void NewFloorRequested(int32 FloorNum);
	void MoveToFloor(int32 FloorNum, float DeltaTime);

private:
	UPROPERTY(EditAnywhere)
	float DoorMoveSpeed = 20.f;
	FVector LargeDoorClosePos;
	FVector SmallDoorClosePos;
	FVector LargeDoorOpenPos;
	FVector SmallDoorOpenPos;
	FVector LargeDoorTarget;
	FVector SmallDoorTarget;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> ElevatorMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> LargeDoor;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> SmallDoor;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<class AElevatorFloor>> Floors;

	int32 CurrentFloor = 1;
	int32 NextFloor = 0;

	EElevatorState ElevatorState = EElevatorState::IDLE;

	UPROPERTY(EditAnywhere)
	float ElevatorMoveSpeed = 20.f;

	void OpenDoors();
	void CloseDoors();
};
