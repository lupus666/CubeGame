// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CubeGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class CUBEGAME_API ACubeGameStateBase : public AGameStateBase
{
	GENERATED_BODY()


public:
	ACubeGameStateBase();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialParameterCollection* > PortalCollections;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int PortalCount;

public:
	TArray<UMaterialParameterCollection*> GetPortalCollections() const
	{
		return PortalCollections;
	}

	int GetPortalCount() const
	{
		return PortalCount;
	}
};
