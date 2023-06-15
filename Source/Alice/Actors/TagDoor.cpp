// Fill out your copyright notice in the Description page of Project Settings.


#include "TagDoor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ATagDoor::ATagDoor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall Mesh"));
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame Mesh"));
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	Knob = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Knob Mesh"));
	DoorOpenTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Door Open Timeline"));
	DoorLockedTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Door Locked Timeline"));

	Wall->SetupAttachment(RootComponent);
	DoorFrame->SetupAttachment(Wall);
	Door->SetupAttachment(DoorFrame);
	Knob->SetupAttachment(Door);

}

void ATagDoor::SpawnGoalRoom()
{
	if (GoalRoomClass)
	{
		FActorSpawnParameters SpawnParams;
		GoalRoom = GetWorld()->SpawnActor<AActor>(GoalRoomClass, GetActorLocation(), FRotator(0.f, GetActorRotation().Yaw + 180.f, 0.f), SpawnParams);
	}
}

void ATagDoor::BeginPlay()
{
	Super::BeginPlay();
	
	OpenUpdateFloat.BindDynamic(this, &ATagDoor::UpdateDoorRotation);
	LockedUpdateFloat.BindDynamic(this, &ATagDoor::UpdateDoorRotation);
	EventTest.BindDynamic(this, &ATagDoor::EventUpdate);

	if (DoorOpenFloatCurve && DoorOpenTimeline)
	{
		DoorOpenTimeline->AddInterpFloat(DoorOpenFloatCurve, OpenUpdateFloat);
		DoorOpenTimeline->AddEvent(0.1f, EventTest);
	}

	if (DoorLockedFloatCurve && DoorLockedTimeline)
	{
		DoorLockedTimeline->AddInterpFloat(DoorLockedFloatCurve, LockedUpdateFloat);
	}
}

void ATagDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATagDoor, bIsOpen);
}

void ATagDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATagDoor::Interact_Implementation(UPrimitiveComponent* HitComponent)
{
	if (bIsLocked)
	{
		MulticastLockedDoor();
	}
	else
	{
		if (!GoalRoom)
		{
			SpawnGoalRoom();
		}

		bIsOpen = !bIsOpen;
		if (bIsOpen)
		{
			DoorOpenTimeline->Play();
			if (OpenCue) UGameplayStatics::PlaySoundAtLocation(this, OpenCue, GetActorLocation());
		}
		else
		{
			DoorOpenTimeline->Reverse();
			if (CloseCue) UGameplayStatics::PlaySoundAtLocation(this, CloseCue, GetActorLocation());
		}
	}
}

void ATagDoor::OnRep_bIsOpen()
{
	if (bIsOpen)
	{
		DoorOpenTimeline->Play();
		if (OpenCue) UGameplayStatics::PlaySoundAtLocation(this, OpenCue, GetActorLocation());
	}
	else
	{
		DoorOpenTimeline->Reverse();
		if (CloseCue) UGameplayStatics::PlaySoundAtLocation(this, CloseCue, GetActorLocation());
	}
}

void ATagDoor::UpdateDoorRotation(float Output)
{
	Door->SetRelativeRotation(FRotator(0.0f, Output, 0.f));
}

void ATagDoor::EventUpdate()
{
	if (!DoorOpenTimeline->IsReversing()) UE_LOG(LogTemp, Warning, TEXT("Event triggered: %f"), DoorOpenTimeline->GetPlaybackPosition());
}

void ATagDoor::MulticastLockedDoor_Implementation()
{
	DoorLockedTimeline->PlayFromStart();
	if (LockedCue) UGameplayStatics::PlaySoundAtLocation(this, LockedCue, GetActorLocation());
}

