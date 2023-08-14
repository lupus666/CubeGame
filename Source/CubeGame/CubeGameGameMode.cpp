// Copyright Epic Games, Inc. All Rights Reserved.

#include "CubeGameGameMode.h"
#include "CubeGame/Character/CubeGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACubeGameGameMode::ACubeGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Character/BP_CubeCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
