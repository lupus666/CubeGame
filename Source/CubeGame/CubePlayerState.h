// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CubePlayerState.generated.h"

class APortal;
/**
 * 
 */
UCLASS()
class CUBEGAME_API ACubePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACubePlayerState();
	
	virtual void BeginPlay() override;
	
	void UpdatePortalState();

protected:
	// virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(VisibleAnywhere)
	TSet<int> InPortals;

	UPROPERTY(VisibleAnywhere)
	TArray<bool> PortalStates;

public:
	TSet<int> GetInPortals() const
	{
		return InPortals;
	}
	
	TArray<bool> GetPortalStates() const
	{
		return PortalStates;
	}
};
