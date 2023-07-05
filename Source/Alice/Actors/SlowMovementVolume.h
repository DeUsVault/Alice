// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlowMovementVolume.generated.h"

UCLASS()
class ALICE_API ASlowMovementVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlowMovementVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void ReduceMovementSpeed(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void RestoreMovementSpeed(AActor* OverlappedActor, AActor* OtherActor);

public:

};
