// Fill out your copyright notice in the Description page of Project Settings.


#include "ElevatorFloor.h"
#include "Elevator.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

AElevatorFloor::AElevatorFloor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	Frame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame"));
	SetRootComponent(Frame);
	
	LargeDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Large Door"));
	LargeDoor->SetupAttachment(RootComponent);
	SmallDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Small Door"));
	SmallDoor->SetupAttachment(RootComponent);
	
	Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Call Button"));
	Button->SetupAttachment(RootComponent);
	ButtonOverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Button Overlap Sphere"));
	ButtonOverlapSphere->SetupAttachment(Button);

	DoorOpenTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Door Open Timeline"));
}

void AElevatorFloor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AElevatorFloor, bCallButtonPressed);
}

void AElevatorFloor::BeginPlay()
{
	Super::BeginPlay();

	FloorState = EElevatorFloorState::CLOSED;

	UpdateTimelineFloat.BindDynamic(this, &AElevatorFloor::UpdateDoorPosition);

	if (DoorOpenFloatCurve)
	{
		DoorOpenTimeline->AddInterpFloat(DoorOpenFloatCurve, UpdateTimelineFloat);
	}
}

void AElevatorFloor::OpenDoors()
{
	DoorOpenTimeline->Play();
	FloorState = EElevatorFloorState::OPENING;
}

void AElevatorFloor::CloseDoors()
{
	DoorOpenTimeline->Reverse();
	FloorState = EElevatorFloorState::CLOSING;
}

void AElevatorFloor::Interact_Implementation(UPrimitiveComponent* HitComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("Character is calling elevator on floor %d"), FloorNumber);
	if (Elevator)
	{
		Elevator->NewFloorRequested(FloorNumber);
	}
}

// Should only be called from Server (OnRep for clients)
void AElevatorFloor::SetCallButtonPressed(bool pressed)
{
	bCallButtonPressed = pressed;
	Button->SetVisibility(!pressed);

	if (pressed && CallButtonCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CallButtonCue, Button->GetComponentLocation());
	}
}

void AElevatorFloor::OnRep_bCallButtonPressed()
{
	Button->SetVisibility(!bCallButtonPressed);
	if (bCallButtonPressed && CallButtonCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CallButtonCue, Button->GetComponentLocation());
	}
}

void AElevatorFloor::UpdateDoorPosition(float Output)
{
	LargeDoor->SetRelativeLocation(FVector(Output, 0.f, 0.f));
	SmallDoor->SetRelativeLocation(FVector(Output * 2, 0.f, 0.f));
}

