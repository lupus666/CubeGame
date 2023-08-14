// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeAbilityBase.h"
#include "CubeAbilityRadialImpulse.generated.h"

/**
 * 
 */
UCLASS()
class CUBEGAME_API ACubeAbilityRadialImpulse : public ACubeAbilityBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDistance = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinChargeTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxChargeTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinStrength = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxStrength = 1000.0f;

	float ChargeTime;
	
protected:
	virtual void BeginPlay() override;

	void GetTargets();

	void RadialImpulse();
	
	UPROPERTY()
	TArray<AActor* > Targets;
	
	float Distance;
	
	float Strength;

public:
	virtual void Tick(float DeltaSeconds) override;

	float LerpByCharge(float Min, float Max);
	
};
