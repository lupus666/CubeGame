// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeAbilityBase.h"
#include "CubeAbilityGrab.generated.h"

/**
 * 
 */
UCLASS()
class CUBEGAME_API ACubeAbilityGrab : public ACubeAbilityBase
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* GrabTarget;
	
public:
	ACubeAbilityGrab();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrabDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrabRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoldDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bConsiderVelocity = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThrowDistance = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThrowRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThrowStiffness = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThrowDamper = 0.1f;
	
protected:
	virtual void BeginPlay() override;

public:
	void FindGrabTarget();

	void ThrowGrabTarget();

	void ReleaseGrabTarget();

	AActor* GetGrabTarget() const;
	
	void SetGrabTarget(AActor* GrabTarget);
	
	virtual void Tick(float DeltaSeconds) override;

};
