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
	void RespawnCharacter(AActor* Actor);

	UFUNCTION()
	void RespawnEssentialObject(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	bool IsPreconditionMet();

	UFUNCTION()
	void CheatingSpawn();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> CheatingBridgeClass;
	
protected:
	virtual void BeginPlay() override;
};



