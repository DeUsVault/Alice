// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowMovementVolume.h"
#include "Alice/AliceCharacter.h"
#include "Alice/TaggerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASlowMovementVolume::ASlowMovementVolume()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ASlowMovementVolume::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnActorBeginOverlap.AddDynamic(this, &ASlowMovementVolume::ReduceMovementSpeed);
		OnActorEndOverlap.AddDynamic(this, &ASlowMovementVolume::RestoreMovementSpeed);
	}
}

void ASlowMovementVolume::ReduceMovementSpeed(AActor* OverlappedActor, AActor* OtherActor)
{
	if (AAliceCharacter* AliceCharacter = Cast<AAliceCharacter>(OtherActor))
	{
		AliceCharacter->MulticastUpdateMovementSpeed(true);
	}
	else if (ATaggerCharacter* TaggerCharacter = Cast<ATaggerCharacter>(OtherActor))
	{
		TaggerCharacter->MulticastUpdateMovementSpeed(true);
	}
}

void ASlowMovementVolume::RestoreMovementSpeed(AActor* OverlappedActor, AActor* OtherActor)
{
	if (AAliceCharacter* AliceCharacter = Cast<AAliceCharacter>(OtherActor))
	{
		AliceCharacter->MulticastUpdateMovementSpeed(false);
	}
	else if (ATaggerCharacter* TaggerCharacter = Cast<ATaggerCharacter>(OtherActor))
	{
		TaggerCharacter->MulticastUpdateMovementSpeed(false);
	}
}
