// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CubeAbilityBase.generated.h"

UCLASS()
class CUBEGAME_API ACubeAbilityBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeAbilityBase();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ACubeGameCharacter* CubeGameCharacter;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void AddImpulse(AActor* Actor, FVector Impulse, FVector AngleImpulse, bool bIgnoreMass);

	virtual void AddImpulseToward(AActor* Actor, FVector TargetImpulse, float LinearStiffness = 20.0f, float AngularDamper = 0.05f);
	
	virtual void AddForce(AActor* Actor, FVector Force, FVector Torque, bool bIgnoreMass);

	virtual void AddForceToward(AActor* Actor, FVector TargetForce, float LinearStiffness = 20.0f, float AngularDamper = 0.05f);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool IsValidTarget(AActor* Actor);

	virtual TArray<AActor*> FilterTargets(TArray<AActor*>& Targets);

	virtual TArray<AActor*> FilterHitTargets(TArray<FHitResult*>& HitResults);

	virtual void GetTargetVelocity(AActor* Actor, FVector& Velocity, FVector& AngularVelocity);

	virtual FVector GetTargetGravity(AActor* Actor);

	virtual void Initialize(ACubeGameCharacter* Character);
};
