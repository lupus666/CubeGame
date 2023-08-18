// Fill out your copyright notice in the Description page of Project Settings.


#include "WindComponent.h"

#include "CubeGame/Character/CubeGameCharacter.h"
#include "WindField.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UWindComponent::UpdateCurrentWindLoad()
{
	ForceSpring.bPrevTargetValid = false;
	CurrentWindForce = UKismetMathLibrary::VectorSpringInterp(CurrentWindForce, TargetWindForce, ForceSpring, 20, 1, GetWorld()->GetDeltaSeconds());
	TorqueSpring.bPrevTargetValid = false;
	CurrentWindTorque = UKismetMathLibrary::VectorSpringInterp(CurrentWindTorque, TargetWindTorque, TorqueSpring, 20, 1, GetWorld()->GetDeltaSeconds());
}

void UWindComponent::BeginPlay()
{
	Super::BeginPlay();
}
