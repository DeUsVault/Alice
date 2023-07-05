// Fill out your copyright notice in the Description page of Project Settings.


#include "AlicePlayerController.h"
#include "Alice/HUD/AliceHUD.h"
#include "Net/UnrealNetwork.h"

void AAlicePlayerController::AddCharacterOverlay()
{
	AliceHUD = Cast<AAliceHUD>(GetHUD());
	if (AliceHUD)
	{
		if (Team == ETeam::ET_Tagger)
		{
			AliceHUD->AddTaggerOverlay();
		}
		else
		{
			AliceHUD->AddCharacterOverlay();
		}
	}
}

void AAlicePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAlicePlayerController, Team);
}

void AAlicePlayerController::OnRep_Team()
{
	UE_LOG(LogTemp, Warning, TEXT("Team Changed to %s"), *UEnum::GetValueAsName(Team).ToString());
	AddCharacterOverlay();
}
