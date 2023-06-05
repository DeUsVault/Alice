// Fill out your copyright notice in the Description page of Project Settings.


#include "ElevatorFloor.h"
#include "Elevator.h"
#include "Components/SphereComponent.h"

AElevatorFloor::AElevatorFloor()
{
	PrimaryActorTick.bCanEverTick = true;
	
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
	ButtonOverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ButtonOverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AElevatorFloor::BeginPlay()
{
	Super::BeginPlay();

	FloorState = EElevatorFloorState::CLOSED;

	LargeDoorOpenPos = FVector(48.f, 0.f, 0.f);
	SmallDoorOpenPos = FVector(95.f, 0.f, 0.f);

	if (HasAuthority())
	{
		ButtonOverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ButtonOverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		ButtonOverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AElevatorFloor::OnSphereOverlap);
		//ButtonOverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AElevatorFloor::OnSphereEndOverlap);
	}
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

void AElevatorFloor::CallElevator()
{
	if (Elevator)
	{
		Elevator->NewFloorRequested(FloorNumber);
	}
}

void AElevatorFloor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Character is at elevator on floor %d"), FloorNumber);
	CallElevator();
}

