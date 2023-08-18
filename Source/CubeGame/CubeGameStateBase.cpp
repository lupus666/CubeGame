// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeGameStateBase.h"

#include "Kismet/GameplayStatics.h"

ACubeGameStateBase::ACubeGameStateBase()
{
	
}

void ACubeGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsWithTag(this, "PrerequisiteActor", PrerequisiteActors);
	UGameplayStatics::GetAllActorsWithTag(this, "TargetPortal", TargetActor);
}
