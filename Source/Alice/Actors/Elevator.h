// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Elevator.generated.h"

UENUM(BlueprintType)
enum class EElevatorState : uint8 {
	IDLE					UMETA(DisplayName = "Idle"),
	MOVING_UP				UMETA(DisplayName = "Moving Up"),
	MOVING_DOWN				UMETA(DisplayName = "Moving Down"),
	DOORS_OPENING			UMETA(DisplayName = "Doors Opening"),
	DOORS_CLOSING			UMETA(DisplayName = "Doors Closing"),
	WAITING					UMETA(DisplayName = "Waiting For Occupants")
};

UCLASS()
class ALICE_API AElevator : public AActor
{
	GENERATED_BODY()
	
public:	
	AElevator();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> FloorOverlapBox;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<class AElevatorFloor>> Floors;

	TQueue<int32> FloorQueue;

	UPROPERTY(Replicated, VisibleAnywhere)
	int32 CurrentFloor = 1;
	UPROPERTY(Replicated, VisibleAnywhere)
	int32 TargetFloor = 0;

	UPROPERTY(Replicated, VisibleAnywhere)
	EElevatorState ElevatorState = EElevatorState::IDLE;

	UPROPERTY(EditAnywhere)
	float ElevatorMoveSpeed = 20.f;

	UPROPERTY(EditAnywhere)
	float ElevatorWaitTime = 5.f;

	FTimerHandle WaitTimer;

	UFUNCTION()
	void OnFloorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OpenDoors();
	void CloseDoors();
};
