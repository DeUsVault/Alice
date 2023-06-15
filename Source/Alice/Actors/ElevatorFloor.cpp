// Fill out your copyright notice in the Description page of Project Settings.


#include "ElevatorFloor.h"
#include "Elevator.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

AElevatorFloor::AElevatorFloor()
{
	PrimaryActorTick.bCanEverTick = true;
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

	LargeDoorOpenPos = FVector(48.f, 0.f, 0.f);
	SmallDoorOpenPos = FVector(95.f, 0.f, 0.f);
}

void AElevatorFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FloorState == EElevatorFloorState::OPENING || FloorState == EElevatorFloorState::CLOSING)
	{
		LargeDoor->SetRelativeLocation(FMath::VInterpConstantTo(LargeDoor->GetRelativeLocation(), LargeDoorTarget, DeltaTime, DoorMoveSpeed));
		SmallDoor->SetRelativeLocation(FMath::VInterpConstantTo(SmallDoor->GetRelativeLocation(), SmallDoorTarget, DeltaTime, DoorMoveSpeed * 2)); // Small door moves twice as far
		if (FVector::Dist(LargeDoor->GetRelativeLocation(), LargeDoorTarget) < 1.f)
		{
			FloorState = FloorState == EElevatorFloorState::OPENING ? EElevatorFloorState::OPEN : EElevatorFloorState::CLOSED;
		}
	}
}

void AElevatorFloor::OpenDoors()
{
	LargeDoorTarget = LargeDoorOpenPos;
	SmallDoorTarget = SmallDoorOpenPos;
	FloorState = EElevatorFloorState::OPENING;
}

void AElevatorFloor::CloseDoors()
{
	LargeDoorTarget = FVector::ZeroVector;
	SmallDoorTarget = FVector::ZeroVector;
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

