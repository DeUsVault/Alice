// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserGate.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Alice/AliceCharacter.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ALaserGate::ALaserGate()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	EntryCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Entry Collision Box"));
	EntryCollisionBox->SetupAttachment(RootComponent);
	DeathCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Death Collision Box"));
	DeathCollisionBox->SetupAttachment(RootComponent);

	Lasers.Reserve(NumBeams);
}

void ALaserGate::BeginPlay()
{
	Super::BeginPlay();

	EntryCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALaserGate::EnteredGate);
	if (HasAuthority())
	{
		DeathCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALaserGate::LeftGate);
	}
	

	FVector ForwardVector = GetActorForwardVector();
	FVector TraceStart = GetActorLocation() + ForwardVector * 5.f; // Offset TraceStart to prevent hit at laser starting point
	FVector TraceEnd = GetActorLocation() + ForwardVector * MaxTraceDistance;

	UWorld* World = GetWorld();
	if (World)
	{
		FHitResult Hit;
		World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility);

		if (Hit.bBlockingHit && LaserNiagaraSystem)
		{
			for (int i = 0; i < NumBeams; i++)
			{
				UNiagaraComponent* Beam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, LaserNiagaraSystem, GetActorLocation() + BeamOffset * i);
				if (Beam)
				{
					Beam->SetColorParameter("LaserColour", FColor::Blue);
					Beam->SetVectorParameter("LaserEnd", Hit.ImpactPoint + BeamOffset * i);
					Lasers.Add(Beam);
				}
			}
		}
	}
}

// Only bound on server in BeginPlay
void ALaserGate::LeftGate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAliceCharacter* AliceCharacter = Cast<AAliceCharacter>(OtherActor);
	if (AliceCharacter && AliceCharacter->HasEnteredGate())
	{
		AliceCharacter->Destroy();
	}
}

void ALaserGate::EnteredGate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAliceCharacter* AliceCharacter = Cast<AAliceCharacter>(OtherActor);
	if (AliceCharacter && !AliceCharacter->HasEnteredGate())
	{
		AliceCharacter->SetHasEnteredGate(true);
		if (LaserEntryCue) UGameplayStatics::PlaySoundAtLocation(this, LaserEntryCue, AliceCharacter->GetActorLocation());

		if (AliceCharacter->IsLocallyControlled())
		{
			for (TActorIterator<ALaserGate> LaserGate(GetWorld()); LaserGate; ++LaserGate)
			{
				LaserGate->SetLaserColour(FColor::Red);
			}
		}
	}
}

void ALaserGate::SetLaserColour(FColor Colour)
{
	for (UNiagaraComponent* Beam : Lasers)
	{
		Beam->SetColorParameter("LaserColour", Colour);
	}
}

