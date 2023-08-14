// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeAnimInstance.h"

#include "CyberCube.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UCubeAnimInstance::UpdateVelocity()
{

	Velocity = CubePawn->GetVelocity();
	// UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_VectorToString(Velocity));
	Speed = UKismetMathLibrary::VSizeXY(Velocity);
}

void UCubeAnimInstance::UpdateRotaion()
{
	Rotation = CubePawn->GetActorRotation();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Rotation);
}

void UCubeAnimInstance::UpdateAcceleration()
{
	if (MovementComponent)
	{
		Acceleration = MovementComponent->GetCurrentAcceleration();
	}
}

void UCubeAnimInstance::UpdateCharacterState()
{
	if (ACubeGameCharacter* Character = Cast<ACubeGameCharacter>(CubePawn))
	{
		if (MovementComponent)
		{
			bIsInAir = MovementComponent->IsFalling();
		}
		bIsOnMount = Character->bIsMounted;
		AnimSpeed = Character->AnimSpeed;
		// bIsJumping = Character.
	}
	if (ACyberCube* CyberCube = Cast<ACyberCube>(CubePawn))
	{
		
	}
}

void UCubeAnimInstance::UpdateAim()
{
	FRotator AimRotator = CubePawn->GetBaseAimRotation();
	FRotator CharacterRotator = CubePawn->GetActorRotation();
	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(AimRotator, CharacterRotator);
	Roll = DeltaRotator.Roll;
	Pitch = DeltaRotator.Pitch;
	Yaw = DeltaRotator.Yaw;
}

void UCubeAnimInstance::ChangeShape(EShapeType ShapeType)
{
	if (!bIsMorphing)
	{
		for (int i = 0; i != ShapeState.Num(); ++i)
		{
			if (i == ShapeType)
			{
				ShapeState[i] = true;
				bIsMorphing = true;
				MorphTime = 0.0f;
			}
			else
			{
				ShapeState[i] = false;
			}
		}
	}
}

void UCubeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	ShapeState.Init(false, EShapeType::SHAPE_TYPE_COUNT);
	
	CubePawn = Cast<APawn>(GetOwningActor());
	if (ACubeGameCharacter* Character = Cast<ACubeGameCharacter>(CubePawn))
	{
		MovementComponent = Character->GetCharacterMovement();
	}
	if (ACyberCube* CyberCube = Cast<ACyberCube>(CubePawn))
	{
		
	}
}

void UCubeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(CubePawn))
	{
		UpdateVelocity();
		UpdateRotaion();
		UpdateAcceleration();
		UpdateAim();
		UpdateCharacterState();
	}
	if (bIsMorphing && MorphTime < 1.0f)
	{
		MorphTime += DeltaSeconds;
	}
	if (MorphTime > 1.0f)
	{
		bIsMorphing = false;
	}
}
