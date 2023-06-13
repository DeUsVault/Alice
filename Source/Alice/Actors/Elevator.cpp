// Fill out your copyright notice in the Description page of Project Settings.


#include "Elevator.h"
#include "ElevatorFloor.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"

// Sets default values
AElevator::AElevator()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ElevatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elevator Mesh"));
	SetRootComponent(ElevatorMesh);
	FloorOverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Floor Overlap Box"));
	FloorOverlapBox->SetupAttachment(RootComponent);
	LargeDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Large Door"));
	LargeDoor->SetupAttachment(RootComponent);
	SmallDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Small Door"));
	SmallDoor->SetupAttachment(RootComponent);
}

void AElevator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AElevator, CurrentFloor);
	DOREPLIFETIME(AElevator, TargetFloor);
	DOREPLIFETIME(AElevator, ElevatorState);
}

void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
	LargeDoorClosePos = LargeDoor->GetRelativeLocation();
	SmallDoorClosePos = SmallDoor->GetRelativeLocation();
	LargeDoorOpenPos = LargeDoorClosePos + FVector(48.f, 0.f, 0.f);
	SmallDoorOpenPos = SmallDoorClosePos + FVector(95.f, 0.f, 0.f);
	if (HasAuthority())
	{
		FloorOverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AElevator::OnFloorOverlap);
	}
	
}

void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ElevatorState == EElevatorState::MOVING_UP || ElevatorState == EElevatorState::MOVING_DOWN)
	{
		MoveToFloor(TargetFloor, DeltaTime);
	}

	if (ElevatorState == EElevatorState::DOORS_OPENING || ElevatorState == EElevatorState::DOORS_CLOSING)
	{
		LargeDoor->SetRelativeLocation(FMath::VInterpConstantTo(LargeDoor->GetRelativeLocation(), LargeDoorTarget, DeltaTime, DoorMoveSpeed));
		SmallDoor->SetRelativeLocation(FMath::VInterpConstantTo(SmallDoor->GetRelativeLocation(), SmallDoorTarget, DeltaTime, DoorMoveSpeed * 2)); // Small door moves twice as far
		if (FVector::Dist(LargeDoor->GetRelativeLocation(), LargeDoorTarget) < 1.f) // Doors finished moving, account for overshoot
		{
			LargeDoor->SetRelativeLocation(LargeDoorTarget);
			SmallDoor->SetRelativeLocation(SmallDoorTarget);
			
			if (HasAuthority())
			{
				if (ElevatorState == EElevatorState::DOORS_OPENING)
				{
					// Doors Opened
					ElevatorState = EElevatorState::WAITING;
					GetWorldTimerManager().SetTimer(WaitTimer, this, &AElevator::CloseDoors, ElevatorWaitTime);
				}
				else
				{
					// Doors Closed, Check for queued calls
					if (FloorQueue.IsEmpty())
					{
						ElevatorState = EElevatorState::IDLE;
					}
					else
					{
						FloorQueue.Dequeue(TargetFloor);
						ElevatorState = TargetFloor > CurrentFloor ? EElevatorState::MOVING_UP : EElevatorState::MOVING_DOWN;
					}
				}
			}
		}
	}
}

void AElevator::MoveToFloor(int32 FloorNum, float DeltaTime)
{
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), Floors[FloorNum-1]->GetActorLocation(), DeltaTime, ElevatorMoveSpeed));
	if (GetActorLocation() == Floors[FloorNum - 1]->GetActorLocation())
	{
		// Reached TargetFloor
		CurrentFloor = FloorNum;
		TargetFloor = 0;
		Floors[CurrentFloor - 1]->SetCallButtonPressed(false);
		OpenDoors();
	}
}

// Called from ElevatorFloor only on server
void AElevator::NewFloorRequested(int32 FloorNum)
{
	if ((ElevatorState == EElevatorState::IDLE || ElevatorState == EElevatorState::DOORS_CLOSING) && CurrentFloor == FloorNum) // Same floor, open door
	{
		OpenDoors();
	}
	else if (ElevatorState == EElevatorState::IDLE && CurrentFloor != FloorNum) // Go directly to the new floor
	{
		TargetFloor = FloorNum;
		ElevatorState = TargetFloor > CurrentFloor ? EElevatorState::MOVING_UP : EElevatorState::MOVING_DOWN;
		Floors[FloorNum - 1]->SetCallButtonPressed(true);
	}
	else if (FloorNum != CurrentFloor && FloorNum != TargetFloor) // Elevator is busy
	{
		FloorQueue.Enqueue(FloorNum);
		Floors[FloorNum - 1]->SetCallButtonPressed(true);
	}
}

void AElevator::OpenDoors()
{
	LargeDoorTarget = LargeDoorOpenPos;
	SmallDoorTarget = SmallDoorOpenPos;
	Floors[CurrentFloor - 1]->OpenDoors();
	ElevatorState = EElevatorState::DOORS_OPENING;
}

void AElevator::CloseDoors()
{
	LargeDoorTarget = LargeDoorClosePos;
	SmallDoorTarget = SmallDoorClosePos;
	Floors[CurrentFloor - 1]->CloseDoors();
	ElevatorState = EElevatorState::DOORS_CLOSING;
}

void AElevator::OnRep_ElevatorState()
{
	if (ElevatorState == EElevatorState::DOORS_CLOSING)
	{
		CloseDoors();
	}
	else if (ElevatorState == EElevatorState::DOORS_OPENING)
	{
		OpenDoors();
	}
}

void AElevator::OnFloorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AElevatorFloor* Floor = Cast<AElevatorFloor>(OtherActor);
	if (Floor)
	{
		CurrentFloor = Floor->GetFloorNumber();
		UE_LOG(LogTemp, Warning, TEXT("Floor %d"), CurrentFloor);
	}
}

void AElevator::Interact_Implementation(UPrimitiveComponent* HitComponent)
{
	NewFloorRequested(FCString::Atoi(*HitComponent->GetName()));
}
