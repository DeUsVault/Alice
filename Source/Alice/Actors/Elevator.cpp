// Fill out your copyright notice in the Description page of Project Settings.


#include "Elevator.h"
#include "ElevatorFloor.h"

// Sets default values
AElevator::AElevator()
{
	PrimaryActorTick.bCanEverTick = true;

	ElevatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elevator Mesh"));
	SetRootComponent(ElevatorMesh);
	LargeDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Large Door"));
	LargeDoor->SetupAttachment(RootComponent);
	SmallDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Small Door"));
	SmallDoor->SetupAttachment(RootComponent);
}

void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
	LargeDoorClosePos = LargeDoor->GetRelativeLocation();
	SmallDoorClosePos = SmallDoor->GetRelativeLocation();
	LargeDoorOpenPos = LargeDoorClosePos + FVector(48.f, 0.f, 0.f);
	SmallDoorOpenPos = SmallDoorClosePos + FVector(95.f, 0.f, 0.f);
}

void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (NextFloor != 0)
	{
		MoveToFloor(NextFloor, DeltaTime);
	}

	if (ElevatorState == EElevatorState::DOORS_OPENING || ElevatorState == EElevatorState::DOORS_CLOSING)
	{
		LargeDoor->SetRelativeLocation(FMath::VInterpConstantTo(LargeDoor->GetRelativeLocation(), LargeDoorTarget, DeltaTime, DoorMoveSpeed));
		SmallDoor->SetRelativeLocation(FMath::VInterpConstantTo(SmallDoor->GetRelativeLocation(), SmallDoorTarget, DeltaTime, DoorMoveSpeed * 2)); // Small door moves twice as far
		if (LargeDoor->GetRelativeLocation() == LargeDoorTarget)
		{
			ElevatorState = ElevatorState == EElevatorState::DOORS_OPENING ? EElevatorState::WAITING : EElevatorState::IDLE;
		}
	}
}

void AElevator::MoveToFloor(int32 FloorNum, float DeltaTime)
{
	ElevatorState = NextFloor > CurrentFloor ? EElevatorState::GOING_UP : EElevatorState::GOING_DOWN;
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), Floors[FloorNum-1]->GetActorLocation(), DeltaTime, ElevatorMoveSpeed));
	if (GetActorLocation() == Floors[FloorNum - 1]->GetActorLocation())
	{
		OpenDoors();
		Floors[FloorNum - 1]->OpenDoors();
	}
}

void AElevator::NewFloorRequested(int32 FloorNum)
{
	NextFloor = FloorNum;
}

void AElevator::OpenDoors()
{
	LargeDoorTarget = LargeDoorOpenPos;
	SmallDoorTarget = SmallDoorOpenPos;
	ElevatorState = EElevatorState::DOORS_OPENING;
}

void AElevator::CloseDoors()
{
	LargeDoorTarget = FVector::ZeroVector;
	SmallDoorTarget = FVector::ZeroVector;
	ElevatorState = EElevatorState::DOORS_CLOSING;
}

