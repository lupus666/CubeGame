// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalActor.h"
#include "EmitterBase.generated.h"

/**
 * 
 */
UCLASS()
class CUBEGAME_API AEmitterBase : public APortalActor
{
	GENERATED_BODY()

public:

	AEmitterBase();

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UArrowComponent* ArrowComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EmitterInterval = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APortalActor> EmitterActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Strength;
	
protected:

	void Emitter();
};
