// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(false);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Overriden in child classes to provide specialised functionality
void AWeapon::Fire()
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}

	Ammo = FMath::Clamp(Ammo - 1, 0, MagazineCapacity);
}

void AWeapon::AddAmmo(int32 AmmoQuantity)
{
	Ammo = FMath::Clamp(Ammo + AmmoQuantity, 0, MagazineCapacity);
}

