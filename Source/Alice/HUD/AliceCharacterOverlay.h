// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AliceCharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class ALICE_API UAliceCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TimeRemainingText;
};
