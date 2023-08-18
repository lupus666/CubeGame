// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WindField.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "WindComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CUBEGAME_API UWindComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StaticSurfaceArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector TargetWindForce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector TargetWindTorque;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector CurrentWindForce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector CurrentWindTorque;

	FVectorSpringState ForceSpring;

	FVectorSpringState TorqueSpring;
	
	void UpdateCurrentWindLoad();
	
protected:
	virtual void BeginPlay() override;
};
