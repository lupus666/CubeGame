// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalActor.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "WindField.generated.h"

UCLASS()
class CUBEGAME_API AWindField : public APortalActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWindField();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector WindDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneCaptureComponent2D* SceneCaptureDepth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneCaptureComponent2D* SceneCaptureNormal;
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// UTextureRenderTarget2D* RTNormalMap;
	
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

	void AddWindLoad(AActor* Actor);

	void CaptureDepthNormal();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float CalcWindLoadByArea(float WindSurfaceArea) const;

	static TArray<FVector> GetCubeNormals(const FVector& ForwardVector);

	FVector CalcWindForceByBodyInstance(FBodyInstance* BodyInstance) const;

	FVector CalcWindForceByRenderTarget();
};
