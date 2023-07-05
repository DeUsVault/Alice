// Fill out your copyright notice in the Description page of Project Settings.


#include "TaggerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Alice/Interfaces/Interactable.h"
#include "Alice/PlayerController/AlicePlayerController.h"
#include "Alice/Weapons/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Alice/AliceCharacter.h"
#include "Sound/SoundCue.h"
#include "Components/DecalComponent.h"

//////////////////////////////////////////////////////////////////////////
// AAliceCharacter

ATaggerCharacter::ATaggerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetMesh(), FName("head")); // Attach the camera to the mesh
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATaggerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATaggerCharacter, EquippedWeapon);
}

void ATaggerCharacter::MulticastUpdateMovementSpeed_Implementation(bool bSlow)
{
	if (bSlow)
	{
		GetCharacterMovement()->MaxWalkSpeed *= SlowFactor;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed /= SlowFactor;
	}
}

void ATaggerCharacter::Restart()
{
	Super::Restart();

	AAlicePlayerController* PlayerController = Cast<AAlicePlayerController>(Controller);
	if (PlayerController)
	{
		//Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATaggerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (HasAuthority())
	{
		AddDefaultWeapon();
	}
	
}

void ATaggerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//////////////////////////////////////////////////////////////////////////
// Input

void ATaggerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATaggerCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATaggerCharacter::Look);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ATaggerCharacter::InteractButtonPressed);

		//Shoot
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATaggerCharacter::FireButtonPressed);

	}

}

void ATaggerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATaggerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATaggerCharacter::InteractButtonPressed()
{
	// Check if player is actually looking at something interactable locally, then check from Server
	FVector TraceStart = FollowCamera->GetComponentLocation();
	if (Controller)
	{
		FVector TraceEnd = TraceStart + Controller->GetControlRotation().Vector() * InteractRange;

		FHitResult Hit;
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GameTraceChannel2);
		if (bHit)
		{
			ServerInteract();
		}
	}
}

void ATaggerCharacter::ServerInteract_Implementation()
{
	UE_LOG(LogTemp, Display, TEXT("ServerRPC: ServerInteract called by %s"), *GetActorNameOrLabel());
	FVector TraceStart = FollowCamera->GetComponentLocation();
	if (Controller)
	{
		FVector TraceEnd = TraceStart + Controller->GetControlRotation().Vector() * InteractRange;

		FHitResult Hit;
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GameTraceChannel2);
		if (bHit)
		{
			if (Hit.GetActor()->Implements<UInteractable>())
			{
				UE_LOG(LogTemp, Warning, TEXT("Interacting with %s"), *Hit.GetComponent()->GetName());
				IInteractable::Execute_Interact(Hit.GetActor(), Hit.GetComponent());
			}
		}
	}
}

void ATaggerCharacter::FireButtonPressed()
{
	if (EquippedWeapon && !EquippedWeapon->IsEmpty() && bCanFire)
	{
		EquippedWeapon->Fire(); // Decrement ammo and play fire animation
		bCanFire = false; // Fire Delay
		GetWorldTimerManager().SetTimer(FireDelayTimer, [this] {bCanFire = true; }, EquippedWeapon->GetWeaponFireDelay(), false);
		
		// Trace from Camera to Crosshairs for damage calculation
		UWorld* World = GetWorld();
		if (World)
		{
			const FVector TraceStart = FollowCamera->GetComponentLocation();
			FVector TraceEnd;
			if (EquippedWeapon->bUseSpread)
			{
				const int32 RandomSeed = FMath::Rand();
				FRandomStream WeaponRandomStream(RandomSeed);
				const float ConeHalfAngle = FMath::DegreesToRadians(EquippedWeapon->GetWeaponSpread() * 0.5f);
				const FVector ShootDir = WeaponRandomStream.VRandCone(FollowCamera->GetForwardVector(), ConeHalfAngle, ConeHalfAngle);
				TraceEnd = TraceStart + ShootDir * TRACE_LENGTH;
				//DrawDebugLine(World, TraceStart, TraceEnd, FColor::Cyan, true);
			}
			else // Weapon has no Spread
			{
				TraceEnd = FollowCamera->GetComponentLocation() + TRACE_LENGTH * FollowCamera->GetForwardVector();
			}
			
			if (!HasAuthority())
			{
				LocalFire(TraceEnd);
			}

			ServerFire(TraceEnd);
		}
	}
}

void ATaggerCharacter::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	//PlayFireMontage();
	
	// Trace from Gun Muzzle to Crosshairs for trail effect
	UWorld* World = GetWorld();
	if (World)
	{
		FHitResult LocalHit;
		World->LineTraceSingleByChannel(LocalHit, GetFollowCamera()->GetComponentLocation(), TraceHitTarget, ECC_Visibility);

		if (!LocalHit.bBlockingHit)
		{
			LocalHit.ImpactPoint = TraceHitTarget;
		}

		if (HasAuthority() && LocalHit.bBlockingHit)
		{
			AAliceCharacter* AliceCharacter = Cast<AAliceCharacter>(LocalHit.GetActor());
			if (AliceCharacter)
			{
				float DamageToApply = EquippedWeapon->GetBaseDamage();
				if (LocalHit.BoneName == FName("head"))
				{
					DamageToApply *= 2.f;
				}
				UGameplayStatics::ApplyDamage(AliceCharacter, DamageToApply, GetController(), this, UDamageType::StaticClass());
			}
		}

		const USkeletalMeshSocket* MuzzleFlashSocket = EquippedWeapon->GetWeaponMesh()->GetSocketByName("MuzzleFlashSocket");
		if (MuzzleFlashSocket)
		{
			FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(EquippedWeapon->GetWeaponMesh());
			FVector BulletStart = SocketTransform.GetLocation();
			
			if (TrailParticles)
			{
				UParticleSystemComponent* Trail = UGameplayStatics::SpawnEmitterAtLocation(World, TrailParticles, BulletStart, FRotator::ZeroRotator, true);
				if (Trail)
				{
					Trail->SetVectorParameter(FName("Target"), LocalHit.ImpactPoint);
				}
			}

			if (MuzzleFlashSystem)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlashSystem, SocketTransform);
			}

			if (FireSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, BulletStart);
			}

			if (ImpactParticlesSystem && LocalHit.bBlockingHit)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticlesSystem, LocalHit.ImpactPoint, LocalHit.ImpactNormal.Rotation());
			}

			if (BulletHoleMaterial && LocalHit.bBlockingHit)
			{
				UDecalComponent* BulletHoleDecal = UGameplayStatics::SpawnDecalAttached(BulletHoleMaterial, FVector(2.f, 2.f, 2.f), LocalHit.GetComponent(), NAME_None, LocalHit.ImpactPoint, FRotationMatrix::MakeFromX(LocalHit.Normal).Rotator(), EAttachLocation::KeepWorldPosition, 20.f);
				if (BulletHoleDecal)
				{
					BulletHoleDecal->SetFadeScreenSize(0.f);
					BulletHoleDecal->AddRelativeRotation(FRotator(0.f, 0.f, FMath::FRandRange(0.f, 360.f))); // Random rotation about normal
				}
			}
		}
	}
}

void ATaggerCharacter::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void ATaggerCharacter::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (IsLocallyControlled() && !HasAuthority())
	{
		return;
	}
	LocalFire(TraceHitTarget);
}

void ATaggerCharacter::AddDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && DefaultWeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		EquipWeapon(DefaultWeapon);
	}
}

void ATaggerCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		EquippedWeapon->SetOwner(this);
		AttachActorToSocket(EquippedWeapon, FName("GunSocket"));
	}
}

void ATaggerCharacter::OnRep_EquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToSocket(EquippedWeapon, FName("GunSocket"));
		//PlayEquipWeaponSound(EquippedWeapon);
	}
}

void ATaggerCharacter::AttachActorToSocket(AActor* ActorToAttach, FName Socket)
{
	if (GetMesh() && ActorToAttach)
	{
		const USkeletalMeshSocket* SocketToAttach = GetMesh()->GetSocketByName(Socket);
		if (SocketToAttach)
		{
			SocketToAttach->AttachActor(ActorToAttach, GetMesh());
		}
	}
}
