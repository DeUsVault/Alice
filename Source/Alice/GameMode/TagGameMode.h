// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TagGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ALICE_API ATagGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ATagGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AAliceCharacter* ElimmedCharacter, class AAlicePlayerController* VictimController, AAlicePlayerController* AttackerController);
	virtual void PlayerLeftGame(class ABlasterPlayerState* PlayerLeaving);
	virtual void HandleMatchHasStarted() override;
	UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 30.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> TaggerPawnClass;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> AlicePawnClass;

	void ChooseGoalRoom();
	void ChooseTagger();


public:
	float GetCountdownTime() const { return CountdownTime; }
};
