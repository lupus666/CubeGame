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
	Velocity = CyberCube->GetVelocity();
	// UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_VectorToString(Velocity));
	Speed = UKismetMathLibrary::VSizeXY(Velocity);
}

void UCubeAnimInstance::UpdateRotaion()
{
	Rotation = CyberCube->GetActorRotation();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Rotation);
}

void UCubeAnimInstance::UpdateAcceleration()
{
	Acceleration = MovementComponent->GetCurrentAcceleration();
}

void UCubeAnimInstance::UpdateCharacterState()
{
	bIsInAir = MovementComponent->IsFalling();
	bIsOnMount = Character->bIsMounted;
	AnimSpeed = Character->AnimSpeed;
	// bIsJumping = Character.
}

void UCubeAnimInstance::UpdateAim()
{
	FRotator AimRotator = Character->GetBaseAimRotation();
	FRotator CharacterRotator = Character->GetActorRotation();
	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(AimRotator, CharacterRotator);
	Roll = DeltaRotator.Roll;
	Pitch = DeltaRotator.Pitch;
	Yaw = DeltaRotator.Yaw;
}

void UCubeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACubeGameCharacter>(GetOwningActor());
	if (Character != nullptr)
	{
		MovementComponent = Character->GetCharacterMovement();
	}
	CyberCube = Cast<ACyberCube>(GetOwningActor());
	if (CyberCube)
	{
		
	}
}

void UCubeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(CyberCube))
	{
		UpdateVelocity();
		UpdateRotaion();
		// UpdateAcceleration();
		// UpdateAim();
		// UpdateCharacterState();
	}
}
