// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TaggerCharacter.generated.h"

UCLASS()
class ALICE_API ATaggerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

public:
	ATaggerCharacter();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateMovementSpeed(bool bSlow);

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void Restart() override;


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

public:
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:

	void FireButtonPressed();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SlowFactor = 0.5f;

	/*
	Weapons
	*/
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<class AWeapon> EquippedWeapon;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeapon> DefaultWeaponClass;
	void AddDefaultWeapon();
	void EquipWeapon(AWeapon* WeaponToEquip);
	UFUNCTION()
	void OnRep_EquippedWeapon();
	void AttachActorToSocket(AActor* ActorToAttach, FName Socket);
	bool bCanFire = true;
	FTimerHandle FireDelayTimer;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<UParticleSystem> TrailParticles;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<UParticleSystem> MuzzleFlashSystem;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<UParticleSystem> ImpactParticlesSystem;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<class USoundCue> FireSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<UMaterialInterface> BulletHoleMaterial;

	/*
	Interaction
	*/

	UPROPERTY(EditAnywhere)
	float InteractRange = 150.f;
	void InteractButtonPressed();
	UFUNCTION(Server, Reliable) //Should be rate-limited to prevent spamming reliable RPCs on keypress
	void ServerInteract();

};
