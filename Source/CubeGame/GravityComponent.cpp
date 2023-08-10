// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityComponent.h"

#include "CubeAnimInstance.h"
#include "CubeGameCharacter.h"
#include "BaseGizmos/GizmoElementArrow.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values for this component's properties
UGravityComponent::UGravityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGravityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGravityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!GravityAccumulator.Equals(FVector(0, 0, 0)))
	{
		if (ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(GetOwner()))
		{
			for (auto& BodyInstance: CubeGameCharacter->GetMesh()->Bodies)
			{
				BodyInstance->AddForce(GravityAccumulator*BodyInstance->GetBodyMass());
			}

			CubeGameCharacter->RotateCameraToGravity(GravityAccumulator);
		}
		else
		{
			const float Mass = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMass();
			Cast<UPrimitiveComponent>(GetOwner()->GetComponentByClass(UPrimitiveComponent::StaticClass()))->AddForce(GravityAccumulator*Mass, EName::None, true);
		}
		GravityAccumulator = FVector(0, 0, 0);
		GravityPriority = 0;
	}
}

void UGravityComponent::AddGravity(FVector GravityDirection, int Priority)
{
	if (Priority > GravityPriority)
	{
		GravityAccumulator = GravityDirection;
	}
	else if (Priority == GravityPriority)
	{
		GravityAccumulator += GravityDirection;
	}
}

