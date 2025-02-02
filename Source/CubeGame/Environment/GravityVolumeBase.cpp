// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityVolumeBase.h"
#include "CubeGame/Character/CubeGameCharacter.h"

#include "GravityComponent.h"
#include "CubeGame/Character/Ability/CubeAbilityBase.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGravityVolumeBase::AGravityVolumeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}

// Called when the game starts or when spawned
void AGravityVolumeBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGravityVolumeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	GetComponents(PrimitiveComponents);
	for (auto& Component : PrimitiveComponents)
	{
		TArray<AActor*> OverlappingActors;
		Component->GetOverlappingActors(OverlappingActors);
		for (auto& Actor : OverlappingActors)
		{
			if (IsActorValid(Actor))
			{
				if (UGravityComponent* GravityComponent = Cast<UGravityComponent>(Actor->GetComponentByClass(UGravityComponent::StaticClass())))
				{
					if (APortalActor* PortalActor = Cast<APortalActor>(Actor))
					{
						if (IsForceValid(PortalActor))
						{
							GravityComponent->AddGravity(GetGravityDirection()*GravityAccelerate, GravityPriority);
						}
					}
					else
					{
						GravityComponent->AddGravity(GetGravityDirection()*GravityAccelerate, GravityPriority);
					}
				}
			}
		}
	}

}

void AGravityVolumeBase::Activate_Implementation(bool bActivate)
{
	bCanRotate = bActivate;
}

FVector AGravityVolumeBase::GetGravityDirection()
{
	return UKismetMathLibrary::Normal(GetActorRotation().RotateVector(GravityDirection));
}

