// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravityComponent.generated.h"


UCLASS(Blueprintable)
class CUBEGAME_API UGravityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGravityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float GravityPriority;

	FVector GravityAccumulator;

	FVector LastGravityDirection = FVector(0, 0, -1);
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddGravity(FVector GravityDirection, int Priority);
		
};
