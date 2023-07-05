// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

#define TRACE_LENGTH 10000.f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SMG UMETA(DisplayName = "Submachine Gun")
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped")
};

UCLASS()
class ALICE_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Fire();

	UPROPERTY(EditAnywhere, Category = Scatter)
	bool bUseSpread = true;

protected:
	virtual void BeginPlay() override;
	void AddAmmo(int32 AmmoQuantity);


private:

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;


	UPROPERTY(EditAnywhere, Category = Combat)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = Combat)
	EWeaponState WeaponState;

	UPROPERTY(EditAnywhere, Category = Combat)
	float BaseDamage = 20.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	int32 Ammo;

	UPROPERTY(EditAnywhere, Category = Combat)
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = 0.2f;

	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomaticWeapon = true;



	/*
	Scatter
	*/

	UPROPERTY(EditAnywhere, Category = Scatter)
	float SpreadDegrees = 20.f;

public:
	bool IsEmpty() { return Ammo <= 0; }

	// Getters and Setters
	void SetWeaponState(EWeaponState NewWeaponState) { WeaponState = NewWeaponState; }
	float GetWeaponSpread() { return SpreadDegrees; }
	float GetWeaponFireDelay() { return FireDelay; }
	float GetBaseDamage() { return BaseDamage; }
	USkeletalMeshComponent* GetWeaponMesh() { return WeaponMesh; }
};
