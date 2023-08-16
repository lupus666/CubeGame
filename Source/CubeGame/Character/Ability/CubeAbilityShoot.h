// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeAbilityBase.h"
#include "CubeAbilityShoot.generated.h"

/**
 * 
 */
UCLASS()
class CUBEGAME_API ACubeAbilityShoot : public ACubeAbilityBase
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> GrabTargets;

	
public:
	ACubeAbilityShoot();
	
	void FindGrabTarget();

	void ShootGrabTarget();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MaxTargets = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrabDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrabRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoldXY = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoldZ = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ShootDistance = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ShootRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ShootStiffness = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ShootDamper = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxMass = 5.0f;
	
protected:
	virtual void BeginPlay() override;
	
	void AddTarget(AActor* Actor);

	void RemoveTarget(AActor* Actor);

	void FloatTarget();

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual bool IsValidTarget(AActor* Actor) override;;
};
