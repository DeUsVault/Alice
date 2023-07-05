// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserGate.generated.h"

UCLASS()
class ALICE_API ALaserGate : public AActor
{
	GENERATED_BODY()
	
public:	
	ALaserGate();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void LeftGate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EnteredGate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> LaserNiagaraSystem;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> EntryCollisionBox;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> DeathCollisionBox;

	TArray<TObjectPtr<class UNiagaraComponent>> Lasers;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> LaserEntryCue;

	UPROPERTY(EditAnywhere)
	int32 NumBeams;
	UPROPERTY(EditAnywhere)
	FVector BeamOffset;
	UPROPERTY(EditAnywhere)
	float MaxTraceDistance = 1000.f;

	void SetLaserColour(FColor Colour);
};
