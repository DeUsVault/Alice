// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Alice/Interfaces/Interactable.h"
#include "Components/TimelineComponent.h"
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
	void Interact(UPrimitiveComponent* HitComponent);
	virtual void Interact_Implementation(UPrimitiveComponent* HitComponent) override;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	

	void OpenDoors();
	void CloseDoors();

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class AElevator> Elevator;

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

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> ButtonDefaultMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> ButtonPressedMaterial;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> ButtonOverlapSphere;

	UPROPERTY(ReplicatedUsing = OnRep_bCallButtonPressed, VisibleAnywhere)
	bool bCallButtonPressed = false;

	// Sounds

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> CallButtonCue;

	// Animations

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> DoorOpenTimeline;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> DoorOpenFloatCurve;

	FOnTimelineFloat UpdateTimelineFloat;

	UFUNCTION()
	void UpdateDoorPosition(float Output);

// Getters and Setters
public:
	int32 GetFloorNumber() { return FloorNumber; }
	void SetFloorNumber(int32 FloorNum) { FloorNumber = FloorNum; }
	void SetCallButtonPressed(bool pressed);
	UFUNCTION()
	void OnRep_bCallButtonPressed();

};
