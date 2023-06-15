// Fill out your copyright notice in the Description page of Project Settings.


#include "Elevator.h"
#include "ElevatorFloor.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

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
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComponent->SetupAttachment(RootComponent);

	Buttons.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor 1 Button")));
	Buttons.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor 2 Button")));
	Buttons.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor 3 Button")));
	Buttons.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor 4 Button")));
	Buttons.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor 5 Button")));
	Buttons.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor 6 Button")));
	Buttons.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor 7 Button")));

	ButtonDefaultMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Default Button Material"));
	ButtonPressedMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Pressed Button Material"));

	for (UStaticMeshComponent* Button : Buttons)
	{
		Button->SetupAttachment(RootComponent);
	}
}

void AElevator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AElevator, CurrentFloor);
	DOREPLIFETIME(AElevator, TargetFloor);
	DOREPLIFETIME(AElevator, ElevatorState);
	DOREPLIFETIME(AElevator, bButton1Pressed);
	DOREPLIFETIME(AElevator, bButton2Pressed);
	DOREPLIFETIME(AElevator, bButton3Pressed);
	DOREPLIFETIME(AElevator, bButton4Pressed);
	DOREPLIFETIME(AElevator, bButton5Pressed);
	DOREPLIFETIME(AElevator, bButton6Pressed);
	DOREPLIFETIME(AElevator, bButton7Pressed);
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
			AudioComponent->Stop();
			
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
						if (MovementCue)
						{
							AudioComponent->SetSound(MovementCue);
							AudioComponent->Play(0.f);
						}
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
		if (HasAuthority())
		{
			CurrentFloor = FloorNum;
			TargetFloor = 0;
			HandleButtons(FloorNum, false);
			OpenDoors();
		}
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
		HandleButtons(FloorNum, true);
		ElevatorState = TargetFloor > CurrentFloor ? EElevatorState::MOVING_UP : EElevatorState::MOVING_DOWN;
		if (MovementCue)
		{
			AudioComponent->SetSound(MovementCue);
			AudioComponent->Play(0.f);
		}
	}
	else if (FloorNum != CurrentFloor && FloorNum != TargetFloor) // Elevator is busy, queue new floor
	{
		FloorQueue.Enqueue(FloorNum);
		HandleButtons(FloorNum, true);
	}
}

void AElevator::OpenDoors()
{
	if (DoorsOpenCue)
	{
		AudioComponent->SetSound(DoorsOpenCue);
		AudioComponent->Play(0.f);
	}
	if (BellCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BellCue, AudioComponent->GetComponentLocation());
	}
	LargeDoorTarget = LargeDoorOpenPos;
	SmallDoorTarget = SmallDoorOpenPos;
	Floors[CurrentFloor - 1]->OpenDoors();
	ElevatorState = EElevatorState::DOORS_OPENING;
}

void AElevator::CloseDoors()
{
	if (DoorsCloseCue)
	{
		AudioComponent->SetSound(DoorsCloseCue);
		AudioComponent->Play(0.f);
	}
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
	else if (ElevatorState == EElevatorState::MOVING_UP || ElevatorState == EElevatorState::MOVING_DOWN)
	{
		if (MovementCue)
		{
			AudioComponent->SetSound(MovementCue);
			AudioComponent->Play(0.f);
		}
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
	int32 FloorNumber = FCString::Atoi(*HitComponent->GetName());
	NewFloorRequested(FloorNumber);
}

void AElevator::SetElevatorPanelButton(int32 FloorNum, bool bPressed)
{
	if (bPressed)
	{
		Buttons[FloorNum - 1]->SetMaterial(0, ButtonPressedMaterial);
		if (ButtonBeepCue)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ButtonBeepCue, Buttons[FloorNum - 1]->GetComponentLocation());
		}
	}
	else
	{
		Buttons[FloorNum - 1]->SetMaterial(0, ButtonDefaultMaterial);
	}
}

// Call from Server
void AElevator::HandleButtons(int32 FloorNum, bool bPressed)
{
	// Set Panel Buttons
	switch (FloorNum)
	{
	case 1:
		bButton1Pressed = bPressed;
		break;
	case 2:
		bButton2Pressed = bPressed;
		break;
	case 3:
		bButton3Pressed = bPressed;
		break;
	case 4:
		bButton4Pressed = bPressed;
		break;
	case 5:
		bButton5Pressed = bPressed;
		break;
	case 6:
		bButton6Pressed = bPressed;
		break;
	case 7:
		bButton7Pressed = bPressed;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid elevator floor requested: %d"), FloorNum);
	}
	SetElevatorPanelButton(FloorNum, bPressed);
	
	// Set Call Buttons
	Floors[FloorNum - 1]->SetCallButtonPressed(bPressed);

}
