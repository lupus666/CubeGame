// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeAbilityBase.h"
#include "Components/SphereComponent.h"
#include "CubeAbilityDilationDefense.generated.h"

/**
 * 
 */
UCLASS()
class CUBEGAME_API ACubeAbilityDilationDefense : public ACubeAbilityBase
{
	GENERATED_BODY()
	
public:
	ACubeAbilityDilationDefense();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefenseRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefenseAngleDegree = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DilationRatio = 0.3f;
	
protected:
	virtual void BeginPlay() override;
	
	void DilationDefense();
	
	UFUNCTION()
	void OnBeginOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex);

	UPROPERTY()
	TArray<AActor*> OverlappingTargets;
	
	float DefenseAngleCos;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void Initialize(ACubeGameCharacter* Character) override;
	
};
