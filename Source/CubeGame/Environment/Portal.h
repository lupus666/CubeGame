// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class CUBEGAME_API APortal : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PortalPlane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PortalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;
	
public:	
	// Sets default values for this actor's properties
	APortal();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APortal> PortalClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialParameterCollection* > PortalCollections;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int PortalTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PortalRange = 30000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PortalViewDistance = 500000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanItemsOverlap;

	bool bIsPlayerSide;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						  int32 OtherBodyIndex);

	TArray<APortal* > OtherPortals;

	UPROPERTY()
	TArray<AActor* > PortalActors;

	UPROPERTY()
	TArray<AActor* > VisibleActors;

	UPROPERTY()
	TArray<AActor* > NonVisibleActors;

	bool bIsBackSide;

	bool bIsBackSideLast;

	bool bThroughPortal;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void Activate();

	void Transition(bool bActivate);

	void TransitActors();

	void TransitCharacter();

	void ResetSeeActor(AActor* Actor);

	UStaticMeshComponent* GetPortalPlane() const
	{
		return PortalPlane;
	}
};
