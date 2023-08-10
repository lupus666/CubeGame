// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "WindField.generated.h"

UCLASS()
class CUBEGAME_API AWindField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWindField();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator WindDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneCaptureComponent2D* SceneCaptureComponent2D;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// UTextureRenderTarget2D* RTDepthMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WindSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WindStrength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StrengthMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* WindSpeedCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurveTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<AActor*> WindFieldActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsDirectionalWind;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator MinWindRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator MaxWindRotator;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ChangeWindDirection();

	void ApplyWindEffect(AActor* Actor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector CalcWindLoad(float WindSurfaceArea);

	static TArray<FVector> GetCubeNormals(const FVector& ForwardVector);

	float CalcSurfaceArea(FBodyInstance* BodyInstance);
};
