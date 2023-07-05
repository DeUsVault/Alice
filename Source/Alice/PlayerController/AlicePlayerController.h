// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AlicePlayerController.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ET_NoTeam UMETA(DisplayName = "No Team"),
	ET_Alice UMETA(DisplayName = "Alice"),
	ET_Tagger UMETA(DisplayName = "Tagger"),
	
	ET_MAX UMETA(Hidden)
};

/**
 * 
 */
UCLASS()
class ALICE_API AAlicePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void AddCharacterOverlay();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;

protected:

private:
	TObjectPtr<class AAliceHUD> AliceHUD;

	UFUNCTION()
	void OnRep_Team();
};
