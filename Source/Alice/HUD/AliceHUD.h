// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AliceCharacterOverlay.h"
#include "AliceHUD.generated.h"

/**
 * 
 */
UCLASS()
class ALICE_API AAliceHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;
	void AddCharacterOverlay();
	void AddTaggerOverlay();

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> CrosshairCenter;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> CharacterOverlayClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> TaggerOverlayClass;

	TObjectPtr<UAliceCharacterOverlay> CharacterOverlay;
	TObjectPtr<UUserWidget> TaggerOverlay;
};
