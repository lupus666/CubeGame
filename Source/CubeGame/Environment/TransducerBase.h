// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalActor.h"
#include "GameFramework/Actor.h"
#include "TransducerBase.generated.h"

UCLASS()
class CUBEGAME_API ATransducerBase : public APortalActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATransducerBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APortalActor* TriggerActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> TriggerActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AActor* ActivateActor;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
