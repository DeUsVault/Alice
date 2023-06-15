// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Alice/Interfaces/Interactable.h"
#include "TagDoor.generated.h"

UCLASS()
class ALICE_API ATagDoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ATagDoor();

	void UnlockDoor() { bIsLocked = false; }
	void SpawnGoalRoom();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UTimelineComponent> DoorOpenTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UTimelineComponent> DoorLockedTimeline;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	void Interact(UPrimitiveComponent* HitComponent);
	virtual void Interact_Implementation(UPrimitiveComponent* HitComponent) override;

private:
	UPROPERTY(EditAnywhere)
	bool bIsLocked = true;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Wall;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DoorFrame;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Door;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Knob;

	// Goal Room Spawning

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> GoalRoomClass;
	TObjectPtr<AActor> GoalRoom;

	// Door 'Animations'

	UPROPERTY(ReplicatedUsing = OnRep_bIsOpen)
	bool bIsOpen = false;

	UFUNCTION()
	void OnRep_bIsOpen();

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> DoorOpenFloatCurve;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> DoorLockedFloatCurve;
	
	FOnTimelineFloat OpenUpdateFloat;
	FOnTimelineFloat LockedUpdateFloat;
	FOnTimelineEvent EventTest;
	
	UFUNCTION()
	void UpdateDoorRotation(float Output);

	UFUNCTION()
	void EventUpdate();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastLockedDoor();

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> LockedCue;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> OpenCue;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> CloseCue;

};
