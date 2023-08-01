// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CubeMountCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "CubeGameCharacter.generated.h"


UCLASS(config=Game)
class ACubeGameCharacter : public ACubeMountCharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicalAnimationComponent* PhysicalAnimationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* MovementPhysicsConstraint;

public:
	ACubeGameCharacter();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class UTimelineComponent* SprintTimelineComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCurveFloat* SprintCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimMontage* InitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimMontage* TightenMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimSequenceBase* MountAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimSequenceBase* MMBAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UPhysicsAsset* PhysicsAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UAnimInstance* AnimInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BodyName = "Pelvis";
	FName GetBodyName() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MountDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AnimSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Stiffness;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Restitution;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ContactDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RelaxThreshold = 50.f;
	
	bool bIsMounted;

protected:
	UFUNCTION()
	void OnPhysicsInit();

	void SetUpMovementConstraint();

	void UpdateMovementConstraint(float DeltaSeconds);
	
	virtual void MoveForward(float Value) override;

	virtual void MoveRight(float Value) override;

	void BeginSprint();

	void EndSprint();
	
	UFUNCTION()
	void OnSprintTimelineTick(float Value);

	virtual void Jump() override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void BeginTighten() override;
	
	virtual void BeginRelax() override;
	
	void Aim();

	void Mount();

	void Attack();
	
	bool bPreventInput;

	bool bTimeDilation;

	bool bIsAttack;
	
	int TightenCount = 0;

	int RelaxCount = 0;

	float XLimit = 24.75f;

	float YLimit = 24.75f;
	
	float ZLimit = 14.5f;

	float ForwardValue;

	float RightValue;

	FTimerHandle JumpTimerHandle;

	FTimerHandle TightenTimerHandle;

	FTimerHandle RelaxTimerHandle;

	FVector MountLocation;

	FName MountBoneName;
	
	FVectorSpringState MovementSpring;

	FFloatSpringState ConstraintSpring;

	FFloatSpringState StiffnessSpring;

	FFloatSpringState DampingSpring;
	
	FFloatSpringState ForwardSpring;

	FFloatSpringState RightSpring;


public:
	FVector GetMountLocation() const;
	
	FName GetMountBoneName() const;

	float GetCurrentRelaxRate() const { return CurrentRelaxRate; }

	void SetCurrentRelaxRate(float Value) { CurrentRelaxRate = Value; }
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;
};

