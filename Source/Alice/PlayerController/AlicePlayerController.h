// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AlicePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ALICE_API AAlicePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void AddCharacterOverlay();

protected:

private:
	TObjectPtr<class AAliceHUD> AliceHUD;
};
