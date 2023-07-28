// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeGameCharacterBase.h"
#include "CubeMountCharacter.generated.h"

UCLASS()
class CUBEGAME_API ACubeMountCharacter : public ACubeGameCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACubeMountCharacter();

	UPROPERTY()
	class UTimelineComponent* TightenTimelineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCurveFloat* TightenCurve;

	UPROPERTY()
	class UTimelineComponent* DilationTimelineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCurveFloat* DilationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RelaxRate = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TightenTime = 3.0f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void UnMount();

	void TimeDilation();

	UFUNCTION()
	virtual void OnDilationTimelineTick(float Value);

	virtual void BeginTighten();

	virtual void EndTighten();

	virtual void BeginRelax();

	virtual void EndRelax();

	UFUNCTION()
	virtual void OnTightenTimelineTick(float Value);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ACubeMountCharacter* CubeMountCharacter;

	bool bTimeDilation;

	float CurrentRelaxRate;

	int TightenCount = 0;

	int RelaxCount = 0;

	FTimerHandle TightenTimerHandle;

	FTimerHandle RelaxTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void OnMount(ACharacter* MountCharacter);
};
