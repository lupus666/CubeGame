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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialParameterCollection*  PortalCollectionSingle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PortalCount;

public:
	TArray<UMaterialParameterCollection*> GetPortalCollections() const
	{
		return PortalCollections;
	}

	UMaterialParameterCollection* GetPortalCollectionSingle() const
	{
		return PortalCollectionSingle;
	}

	int GetPortalCount() const
	{
		return PortalCount;
	}
};
