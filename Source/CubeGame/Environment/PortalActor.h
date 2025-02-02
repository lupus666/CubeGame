// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Portal.h"
#include "GameFramework/Actor.h"
#include "PortalActor.generated.h"

UCLASS()
class CUBEGAME_API APortalActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MainMesh;

public:	
	// Sets default values for this actor's properties
	APortalActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsInversed = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsStatic = true;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool GetVisibility(bool bIsVisible);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APortal* BelongPortal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UStaticMeshComponent* > ActorMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UShapeComponent* > ShapeComponents;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bVisibility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bTransition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bThroughPortal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsBackSide;

	bool bIsInversionVisibility;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void Initialize(APortal* InPortal);

	void InitStaticMesh(UStaticMeshComponent* StaticMeshComponent);

	void InitStaticShape(UShapeComponent* StaticShapeComponent);

	void Transition(bool bIsVisible);
	
	void TransitActor();

	UFUNCTION(BlueprintNativeEvent)
	void Seen(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	void Seen_Implementation(bool bIsVisible);

	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, APortal* Portal);

	void EndOverlap(UPrimitiveComponent* OverlappedComponent, APortal* Portal);

	bool IsForceValid(const APortalActor* PortalActor) const;

	static bool IsActorValid(const AActor* Actor);
};
