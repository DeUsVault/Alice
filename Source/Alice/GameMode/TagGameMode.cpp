// Fill out your copyright notice in the Description page of Project Settings.


#include "TagGameMode.h"
#include "Alice/AliceCharacter.h"
#include "Alice/PlayerController/AlicePlayerController.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ATagGameMode::ATagGameMode()
{
	bDelayedStart = true;
}

void ATagGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ATagGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ATagGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	/*for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AAlicePlayerController* AlicePlayer = Cast<AAlicePlayerController>(*It);
		if (AlicePlayer)
		{
			AlicePlayer->OnMatchStateSet(MatchState);
		}
	}*/
}

void ATagGameMode::PlayerEliminated(AAliceCharacter* ElimmedCharacter, AAlicePlayerController* VictimController, AAlicePlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	/*AAlicePlayerState* AttackerPlayerState = AttackerController ? Cast<AAlicePlayerState>(AttackerController->PlayerState) : nullptr;
	AAlicePlayerState* VictimPlayerState = VictimController ? Cast<AAlicePlayerState>(VictimController->PlayerState) : nullptr;

	AAliceGameState* AliceGameState = GetGameState<AAliceGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && AliceGameState)
	{
		TArray<ABlasterPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : AliceGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		AttackerPlayerState->AddToScore(1.f);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AAlicePlayerController* AlicePlayer = Cast<AAlicePlayerController>(*It);
		if (AlicePlayer && AttackerPlayerState && VictimPlayerState)
		{
			AlicePlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}*/
}

void ATagGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
	/*if (PlayerLeaving == nullptr) return;
	AAliceGameState* AliceGameState = GetGameState<AAliceGameState>();
	if (AliceGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		AliceGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	AAliceCharacter* CharacterLeaving = Cast<AAliceCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}*/
}