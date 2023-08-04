// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeGameCharacter.h"
#include "CyberCube.h"
#include "Animation/AnimInstance.h"
#include "CubeAnimInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EShapeType
{
	Cube=0		UMETA(DisplayName="Cube"),
	Sphere		UMETA(DisplayName="Sphere"),
	Plane		UMETA(DisplayName="Plane"),

	SHAPE_TYPE_COUNT
};


UCLASS()
class CUBEGAME_API UCubeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void UpdateVelocity();

	void UpdateRotaion();

	void UpdateAcceleration();

	void UpdateCharacterState();
	
	void UpdateAim();

	void ChangeShape(EShapeType ShapeType);
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	APawn* CubePawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCharacterMovementComponent* MovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAnimSequenceBase* MountAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAnimSequenceBase* MMBAnim;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ZSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Roll;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Yaw;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float AnimSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsJumping;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsOnMount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<bool> ShapeState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int JumpCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Acceleration;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float MorphTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsMorphing = false;

};
