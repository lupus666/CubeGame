// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeAbilityBase.h"
#include "Components/SphereComponent.h"
#include "CubeAbilityBlackHole.generated.h"

/**
 * 
 */
UCLASS()
class CUBEGAME_API ACubeAbilityBlackHole : public ACubeAbilityBase
{
	GENERATED_BODY()

public:
	ACubeAbilityBlackHole();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxRadius = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinMaxTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Strength = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StrengthExponent = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnDistance = 1000.0f;

protected:
	virtual void BeginPlay() override;

	void PullTowardsCenter();

	UPROPERTY()
	TArray<AActor*> OverlappingTargets;
	
	float CurrentTime;

	UFUNCTION()
	void OnBeginOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex);

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void Initialize(ACubeGameCharacter* Character) override;
};
