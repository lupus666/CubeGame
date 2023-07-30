// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeGameCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Pinhole.generated.h"

UCLASS()
class CUBEGAME_API APinhole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APinhole();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						  int32 OtherBodyIndex);
};
