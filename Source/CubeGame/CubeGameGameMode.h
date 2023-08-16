// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CubeGameGameMode.generated.h"

UCLASS(minimalapi)
class ACubeGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACubeGameGameMode();

	UFUNCTION()
	void Respawn(AActor* Actor);

protected:
	virtual void BeginPlay() override;
};



