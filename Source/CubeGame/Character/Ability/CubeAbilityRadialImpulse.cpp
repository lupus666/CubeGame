// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeAbilityRadialImpulse.h"

#include "CubeGame/Character/CubeGameCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void ACubeAbilityRadialImpulse::BeginPlay()
{
	Super::BeginPlay();

	ChargeTime = FMath::Clamp((ChargeTime - MinChargeTime) / (MaxChargeTime - MinChargeTime), 0.0, 1.0);
	Distance = LerpByCharge(MinDistance, MaxDistance);
	Strength = LerpByCharge(MinStrength, MaxStrength);
	GetTargets();
	RadialImpulse();
	Destroy();
}

void ACubeAbilityRadialImpulse::GetTargets()
{
	if (CubeGameCharacter)
	{
		const FVector CubeLocation = CubeGameCharacter->GetMesh()->GetBoneLocation(CubeGameCharacter->GetBodyName());
		TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes({
				TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery2),
				TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery3),
				TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery4)}
				);
		TArray<AActor* > OverlappingActors;
		UKismetSystemLibrary::SphereOverlapActors(this, CubeLocation, Distance, ObjectTypes, nullptr, TArray<AActor*>({CubeGameCharacter}), OverlappingActors);
		Targets = FilterTargets(OverlappingActors);
	}
}

void ACubeAbilityRadialImpulse::RadialImpulse()
{
	if(CubeGameCharacter)
	{
		for (auto& Actor: Targets)
		{
			FVector DeltaImpulse = Actor->GetActorLocation() - CubeGameCharacter->GetMesh()->GetBoneLocation(CubeGameCharacter->GetBodyName());
			float DistancePenalty = FMath::Exp(-DeltaImpulse.Size() / Distance);
			AddImpulse(Actor, DeltaImpulse * Strength * DistancePenalty, FVector(0, 0, 0), false);
		}
	}
}

void ACubeAbilityRadialImpulse::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

float ACubeAbilityRadialImpulse::LerpByCharge(float Min, float Max)
{
	return FMath::Lerp(Min, Max, ChargeTime);
}
