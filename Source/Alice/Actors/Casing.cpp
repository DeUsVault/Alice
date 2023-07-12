// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Casing Mesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	SetLifeSpan(5.f);
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
	CasingMesh->AddImpulse(GetActorForwardVector() * CasingEjectionImpulse);

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
}

void ACasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (CasingSound && !bSoundPlayed)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CasingSound, GetActorLocation());
		bSoundPlayed = true;
	}
}

void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

