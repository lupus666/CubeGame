// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalActor.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "GravityVolumeBase.generated.h"

UCLASS()
class CUBEGAME_API AGravityVolumeBase : public APortalActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravityVolumeBase();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector GravityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GravityAccelerate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GravityForce;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GravityPriority;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanRotate;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetGravityDirection();
};
