// Fill out your copyright notice in the Description page of Project Settings.


#include "AlicePlayerController.h"
#include "Alice/HUD/AliceHUD.h"

void AAlicePlayerController::AddCharacterOverlay()
{
	AliceHUD = Cast<AAliceHUD>(GetHUD());
	if (AliceHUD)
	{
		AliceHUD->AddCharacterOverlay();
	}
}
