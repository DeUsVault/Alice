// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Alice/Interfaces/Interactable.h"
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
class ALICE_API AElevator : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AElevator();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetElevatorPanelButton(int32 FloorNum, bool bPressed);
	void HandleButtons(int32 FloorNum, bool bPressed);

public:	
	virtual void Tick(float DeltaTime) override;
	void NewFloorRequested(int32 FloorNum);
	void MoveToFloor(int32 FloorNum, float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	void Interact(UPrimitiveComponent* HitComponent);
	virtual void Interact_Implementation(UPrimitiveComponent* HitComponent) override;

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

	// Buttons
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UStaticMeshComponent>> Buttons;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> ButtonDefaultMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> ButtonPressedMaterial;

	UPROPERTY(ReplicatedUsing = OnRep_bButton1Pressed)
	bool bButton1Pressed{ 0 };
	UPROPERTY(ReplicatedUsing = OnRep_bButton2Pressed)
	bool bButton2Pressed{ 0 };
	UPROPERTY(ReplicatedUsing = OnRep_bButton3Pressed)
	bool bButton3Pressed{ 0 };
	UPROPERTY(ReplicatedUsing = OnRep_bButton4Pressed)
	bool bButton4Pressed{ 0 };
	UPROPERTY(ReplicatedUsing = OnRep_bButton5Pressed)
	bool bButton5Pressed{ 0 };
	UPROPERTY(ReplicatedUsing = OnRep_bButton6Pressed)
	bool bButton6Pressed{ 0 };
	UPROPERTY(ReplicatedUsing = OnRep_bButton7Pressed)
	bool bButton7Pressed{ 0 };

	UFUNCTION()
	void OnRep_bButton1Pressed() { SetElevatorPanelButton(1, bButton1Pressed); }
	UFUNCTION()
	void OnRep_bButton2Pressed() { SetElevatorPanelButton(2, bButton2Pressed); }
	UFUNCTION()
	void OnRep_bButton3Pressed() { SetElevatorPanelButton(3, bButton3Pressed); }
	UFUNCTION()
	void OnRep_bButton4Pressed() { SetElevatorPanelButton(4, bButton4Pressed); }
	UFUNCTION()
	void OnRep_bButton5Pressed() { SetElevatorPanelButton(5, bButton5Pressed); }
	UFUNCTION()
	void OnRep_bButton6Pressed() { SetElevatorPanelButton(6, bButton6Pressed); }
	UFUNCTION()
	void OnRep_bButton7Pressed() { SetElevatorPanelButton(7, bButton7Pressed); }


	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> AudioComponent;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<class AElevatorFloor>> Floors;

	TQueue<int32> FloorQueue;

	UPROPERTY(Replicated, VisibleAnywhere)
	int32 CurrentFloor = 1;
	UPROPERTY(Replicated, VisibleAnywhere)
	int32 TargetFloor = 0;

	UPROPERTY(ReplicatedUsing = OnRep_ElevatorState, VisibleAnywhere)
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

	UFUNCTION()
	void OnRep_ElevatorState();

	// Elevator Sounds

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> DoorsOpenCue;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> DoorsCloseCue;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> MovementCue;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> BellCue;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ButtonBeepCue;
};
