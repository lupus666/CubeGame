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

public:
	ACubeGameCharacter();

	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicalAnimationComponent* PhysicalAnimationComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* MovementPhysicsConstraint;
	
	UPROPERTY()
	class UTimelineComponent* SprintTimelineComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCurveFloat* SprintCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCurveFloat* TorqueCurve;
		
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
	class UPhysicsAsset* CubePhysicsAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UPhysicsAsset* SpherePhysicsAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UPhysicsAsset* PlanePhysicsAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimSequence* CubeSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimSequence* SphereSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimSequence* PlaneSequence;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCubeAnimInstance* AnimInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BodyName = "Pelvis";

	float CurrentTorque;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CenterHeight = 17.5;
	
	bool bIsMounted;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpImpulse;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RollTorque;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxTorque;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxAngularVelocity = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxLinearVelocity = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanJump;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxPower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bConstantPower;

	bool bIsInWindField = false;

	bool bIsSphere = false;

protected:
	UFUNCTION()
	void SetPhysicalAnimation();
	
	void UpdateMovementConstraint(float DeltaSeconds);
	
	virtual void MoveForward(float Value) override;

	virtual void MoveRight(float Value) override;

	void UpdateRootMovement(float DeltaSeconds);
	
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

	void ToSphere();

	UFUNCTION()
	void ToCube();

	void ToPlane();
	
	bool bPreventInput;

	bool bTimeDilation;

	bool bIsAttack;

	bool bIsJumping = false;
	
	int TightenCount = 0;

	int RelaxCount = 0;

	float XLimit = 50.0f;

	float YLimit = 50.0f;
	
	float ZLimit = 50.0f;

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

	FFloatSpringState SphereForwardSpring;

	FVectorSpringState RootMovementSpring;

public:
	FVector GetMountLocation() const;
	
	FName GetMountBoneName() const;

	float GetCurrentRelaxRate() const { return CurrentRelaxRate; }

	void SetCurrentRelaxRate(float Value) { CurrentRelaxRate = Value; }

	void SetUpMovementConstraint();

	void CancelMovementConstraint();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;
};

