// Copyright Epic Games, Inc. All Rights Reserved.

#include "AliceGameMode.h"
#include "AliceCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAliceGameMode::AAliceGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_AliceCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
