// Copyright Epic Games, Inc. All Rights Reserved.

#include "CubeGameGameMode.h"
#include "CubeGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACubeGameGameMode::ACubeGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
