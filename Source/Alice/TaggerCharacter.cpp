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

void ATaggerCharacter::Restart()
{
	Super::Restart();

	AAlicePlayerController* PlayerController = Cast<AAlicePlayerController>(Controller);
	if (PlayerController)
	{
		Cast<AAlicePlayerController>(Controller)->AddCharacterOverlay();

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

