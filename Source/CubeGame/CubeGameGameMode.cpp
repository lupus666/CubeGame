// Copyright Epic Games, Inc. All Rights Reserved.

#include "CubeGameGameMode.h"

#include "CubeGameStateBase.h"
#include "CubePlayerController.h"
#include "CubePlayerState.h"
#include "CubeGame/Character/CubeGameCharacter.h"
#include "Environment/Portal.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

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

void ACubeGameGameMode::RespawnCharacter(AActor* Actor)
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
				CubeGameCharacter->OnDestroyed.AddDynamic(this, &ACubeGameGameMode::RespawnCharacter);
				PlayerController->Possess(CubeGameCharacter);
				PlayerController->UpdateCharacter();
				TArray<AActor* > Portals;
				UGameplayStatics::GetAllActorsOfClass(this, APortal::StaticClass(), Portals);
				for (auto& PortalActor: Portals)
				{
					if (APortal* Portal = Cast<APortal>(PortalActor))
					{
						if (Portal->bIsMainPortal)
						{
							Portal->Activate_Implementation(true);
						}
					}
				}
			}
		}
	}
}

void ACubeGameGameMode::RespawnEssentialObject(AActor* Actor)
{
	if (ACubeGameStateBase* CubeGameStateBase = GetGameState<ACubeGameStateBase>())
	{
		if (const int i = CubeGameStateBase->GetEssentialActors().Find(Actor); i >= 0)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = GetInstigator();
			SpawnInfo.ObjectFlags |= RF_Transient;
			if (AActor* EssentialObject = GetWorld()->SpawnActor<AActor>(CubeGameStateBase->GetEssentialActorClass()[i],
				UKismetMathLibrary::MakeTransform(CubeGameStateBase->GetRespawnLocations()[i], FRotator()),
				SpawnInfo))
			{
				EssentialObject->OnDestroyed.AddDynamic(this, &ACubeGameGameMode::RespawnEssentialObject);
				CubeGameStateBase->GetEssentialActors()[i] = EssentialObject;
			}
		}
	}
}

bool ACubeGameGameMode::IsPreconditionMet()
{
	if (ACubeGameStateBase* CubeGameStateBase = GetGameState<ACubeGameStateBase>())
	{
		for(auto& PrerequisiteActor: CubeGameStateBase->GetPrerequisiteActors())
		{
			if (!IButtonInterface::Execute_IsActivate(PrerequisiteActor))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

void ACubeGameGameMode::CheatingSpawn()
{
	FVector RedVector = FVector(-3713.257824,-3331.843894,761.733775);
	FVector GreenVector = FVector(-835.124936,-2014.008839,5572.471779);
	FVector BlueVector = FVector(-2115.979913,4063.976311,101.746102);
	TArray<FVector> SpawnLocations = TArray<FVector>({
		RedVector, GreenVector, BlueVector
	});

	if (ACubeGameStateBase* CubeGameStateBase = GetGameState<ACubeGameStateBase>())
	{
		for (int i = 0; i < CubeGameStateBase->GetEssentialActorClass().Num(); i++)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = GetInstigator();
			SpawnInfo.ObjectFlags |= RF_Transient;
			if (AActor* EssentialObject = GetWorld()->SpawnActor<AActor>(CubeGameStateBase->GetEssentialActorClass()[i],
				UKismetMathLibrary::MakeTransform(SpawnLocations[i], FRotator()),
				SpawnInfo))
			{
				
			}
		}
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.ObjectFlags |= RF_Transient;
		if (AActor* CheatingBridge = GetWorld()->SpawnActor<AActor>(CheatingBridgeClass,
			UKismetMathLibrary::MakeTransform(FVector(551.377800,531.431841,423.567744), FRotator()),
			SpawnInfo))
		{
			
		}
	}
}

void ACubeGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	CubeGameCharacter->OnDestroyed.AddDynamic(this, &ACubeGameGameMode::RespawnCharacter);

	if (UGameplayStatics::GetCurrentLevelName(this) == "LevelCubeWorld")
	{
		if (ACubeGameStateBase* CubeGameStateBase = GetGameState<ACubeGameStateBase>())
		{
			TArray<AActor* > EssentialActors;
			for (int i = 0; i < CubeGameStateBase->GetEssentialActorClass().Num(); i++)
			{
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.Instigator = GetInstigator();
				SpawnInfo.ObjectFlags |= RF_Transient;
				if (AActor* EssentialObject = GetWorld()->SpawnActor<AActor>(CubeGameStateBase->GetEssentialActorClass()[i],
					UKismetMathLibrary::MakeTransform(CubeGameStateBase->GetRespawnLocations()[i], FRotator()),
					SpawnInfo))
				{
					EssentialObject->OnDestroyed.AddDynamic(this, &ACubeGameGameMode::RespawnEssentialObject);
					EssentialActors.Add(EssentialObject);
				}
			}
			CubeGameStateBase->SetEssentialActors(EssentialActors);
		}
	}
}
