// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalActor.h"
#include "GameFramework/Actor.h"
#include "TransducerBase.generated.h"

UCLASS()
class CUBEGAME_API ATransducerBase : public APortalActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATransducerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
