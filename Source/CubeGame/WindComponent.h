// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WindField.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "WindComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CUBEGAME_API UWindComponent : public USphereComponent
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						  int32 OtherBodyIndex);
};
