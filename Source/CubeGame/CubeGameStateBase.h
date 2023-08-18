// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CubeGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class CUBEGAME_API ACubeGameStateBase : public AGameStateBase
{
	GENERATED_BODY()


public:
	ACubeGameStateBase();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialParameterCollection* > PortalCollections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialParameterCollection*  PortalCollectionSingle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> PrerequisiteActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> EssentialActorClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor* > EssentialActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> RespawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PortalCount;

public:
	TArray<UMaterialParameterCollection*> GetPortalCollections() const
	{
		return PortalCollections;
	}

	UMaterialParameterCollection* GetPortalCollectionSingle() const
	{
		return PortalCollectionSingle;
	}

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetTargetActor() const
	{
		return TargetActor;
	}

	TArray<AActor*> GetPrerequisiteActors() const
	{
		return PrerequisiteActors;
	}

	TArray<TSubclassOf<AActor>> GetEssentialActorClass() const
	{
		return EssentialActorClass;
	}

	TArray<FVector> GetRespawnLocations() const
	{
		return RespawnLocations;
	}

	TArray<AActor*> GetEssentialActors() const
	{
		return EssentialActors;
	}

	void SetEssentialActors(const TArray<AActor*>& Actors)
	{
		this->EssentialActors = Actors;
	}
	
	int GetPortalCount() const
	{
		return PortalCount;
	}
};
