// Fill out your copyright notice in the Description page of Project Settings.


#include "WindComponent.h"

#include "CubeGameCharacter.h"
#include "WindField.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UWindComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UWindComponent::BeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UWindComponent::EndOverlap);
}

void UWindComponent::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AWindField* WindField = Cast<AWindField>(OtherActor))
	{
		WindField->WindFieldActors.Add(this->GetOwner());
		ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(GetOwner());
		if (!CubeGameCharacter->bIsSphere)
		{
			CubeGameCharacter->CancelMovementConstraint();
			CubeGameCharacter->bIsInWindField = true;
		}
	}
}

void UWindComponent::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AWindField* WindField = Cast<AWindField>(OtherActor))
	{
		WindField->WindFieldActors.Remove(this->GetOwner());
		ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(GetOwner());
		if (!CubeGameCharacter->bIsSphere)
		{
			CubeGameCharacter->bIsInWindField = false;
			CubeGameCharacter->SetUpMovementConstraint();
		}
	}
}
