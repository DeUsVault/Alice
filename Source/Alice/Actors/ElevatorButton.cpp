// Fill out your copyright notice in the Description page of Project Settings.


#include "ElevatorButton.h"
#include "Elevator.h"
#include "Components/SphereComponent.h"

AElevatorButton::AElevatorButton()
{
	PrimaryActorTick.bCanEverTick = false;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button Mesh"));
	SetRootComponent(ButtonMesh);

	ButtonOverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Button Overlap Sphere"));
	ButtonOverlapSphere->SetupAttachment(RootComponent);
	ButtonOverlapSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
}

void AElevatorButton::BeginPlay()
{
	Super::BeginPlay();
	
}

void AElevatorButton::Interact_Implementation(UPrimitiveComponent* HitComponent)
{
	if (Elevator)
	{
		Elevator->NewFloorRequested(ButtonFloorNumber);
	}
}

