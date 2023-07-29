// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CubeMountCharacter.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MountDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AnimSpeed = 1;
	
	bool bIsMounted;

protected:
	UFUNCTION()
	void OnPhysicsInit();
	
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

	FTimerHandle JumpTimerHandle;

	FTimerHandle TightenTimerHandle;

	FTimerHandle RelaxTimerHandle;

	FVector MountLocation;

	FName MountBoneName;

public:
	FVector GetMountLocation() const;
	
	FName GetMountBoneName() const;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;
};

