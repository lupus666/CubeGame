// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetMathLibrary.h"

#include "CyberCube.generated.h"

UCLASS()
class CUBEGAME_API ACyberCube : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CyberCube, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Cube;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CyberCube, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CyberCube, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CyberCube, meta = (AllowPrivateAccess = "true"))
	class UPhysicsAsset* PhysicsAsset;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CyberCube, meta = (AllowPrivateAccess = "true"))
	class UPhysicalAnimationComponent* PhysicalAnimationComponent;
	
	UPROPERTY()
	class UTimelineComponent* TightenTimelineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CyberCube, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* TightenCurve;

	UPROPERTY()
	class UTimelineComponent* DilationTimelineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CyberCube, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* DilationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CyberCube, meta = (AllowPrivateAccess = "true"))
	float RelaxRate = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CyberCube, meta = (AllowPrivateAccess = "true"))
	float TightenTime = 3.0f;

	float CurrentRelaxRate;
	
	bool bTimeDilation;

	int TightenCount = 0;

	int RelaxCount = 0;

	FTimerHandle TightenTimerHandle;

	FTimerHandle RelaxTimerHandle;
	
	FName BodyName = "Pelvis";

	FVector CurrentTorque = FVector(0, 0 ,0);
	
	FVectorSpringState ForwardSprintState;

public:
	// Sets default values for this pawn's properties
	ACyberCube();

	/** Vertical impulse to apply when pressing jump */
	UPROPERTY(EditAnywhere, Category=CyberCube)
	float JumpImpulse;

	/** Torque to apply when trying to roll ball */
	UPROPERTY(EditAnywhere, Category=CyberCube)
	float RollTorque;
	
	UPROPERTY(EditAnywhere, Category=CyberCube)
	float MaxTorque;

	UPROPERTY(EditAnywhere, Category=CyberCube)
	float MaxAngularVelocity = 100.0f;

	UPROPERTY(EditAnywhere, Category=CyberCube)
	float MaxLinearVelocity = 30.f;

	bool bCanJump;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void ZoomIn();

	void ZoomOut();

	void TimeDilation();

	UFUNCTION()
	void OnDilationTimelineTick(float Value);

	void BeginTighten();

	void EndTighten();

	void BeginRelax();

	void EndRelax();

	UFUNCTION()
	void OnTightenTimelineTick(float Value);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
