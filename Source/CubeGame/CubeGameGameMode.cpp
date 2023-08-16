// Copyright Epic Games, Inc. All Rights Reserved.

#include "CubeGameGameMode.h"

#include "CubeGameStateBase.h"
#include "CubePlayerController.h"
#include "CubePlayerState.h"
#include "CubeGame/Character/CubeGameCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ACubeGameGameMode::ACubeGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Character/BP_CubeCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PlayerControllerClass = ACubePlayerController::StaticClass();
	GameStateClass = ACubeGameStateBase::StaticClass();
	PlayerStateClass = ACubePlayerState::StaticClass();
}

void ACubeGameGameMode::Respawn(AActor* Actor)
{
	if (ACubePlayerController* PlayerController = Cast<ACubePlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		if (APlayerStart* PlayerStart = Cast<APlayerStart>(FindPlayerStart(PlayerController)))
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = GetInstigator();
			SpawnInfo.ObjectFlags |= RF_Transient;
			if (ACubeGameCharacter* CubeGameCharacter = GetWorld()->SpawnActor<ACubeGameCharacter>(DefaultPawnClass, PlayerStart->GetTransform(), SpawnInfo))
			{
				CubeGameCharacter->OnDestroyed.AddDynamic(this, &ACubeGameGameMode::Respawn);
				PlayerController->Possess(CubeGameCharacter);
			}
		}
	}
}

void ACubeGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	CubeGameCharacter->OnDestroyed.AddDynamic(this, &ACubeGameGameMode::Respawn);
}
