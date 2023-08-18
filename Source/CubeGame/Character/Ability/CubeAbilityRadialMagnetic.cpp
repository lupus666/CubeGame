// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeAbilityRadialMagnetic.h"

#include "CubeGame/Character/CubeGameCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void ACubeAbilityRadialMagnetic::BeginPlay()
{
	Super::BeginPlay();
	
	ChargeTime = FMath::Clamp((ChargeTime - MinChargeTime) / (MaxChargeTime - MinChargeTime), 0.0, 1.0);
	Distance = LerpByCharge(MinDistance, MaxDistance);
	Strength = LerpByCharge(MinStrength, MaxStrength);
	GetTargets();
	RadialGrab();
	Destroy();
}

void ACubeAbilityRadialMagnetic::GetTargets()
{
	if (CubeGameCharacter)
	{
		const FVector CubeLocation = CubeGameCharacter->GetMesh()->GetBoneLocation(CubeGameCharacter->GetBodyName());
		TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes({
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery2),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery3),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery4),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery13),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery14),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery15)}
			);
		TArray<AActor* > OverlappingActors;
		UKismetSystemLibrary::SphereOverlapActors(this, CubeLocation, Distance, ObjectTypes, nullptr, TArray<AActor*>({CubeGameCharacter}), OverlappingActors);
		Targets = FilterTargets(OverlappingActors);
	}
}

void ACubeAbilityRadialMagnetic::RadialGrab()
{
	if(CubeGameCharacter)
	{
		for (auto& Actor: Targets)
		{
			FVector DeltaImpulse = CubeGameCharacter->GetMesh()->GetBoneLocation(CubeGameCharacter->GetBodyName()) - Actor->GetActorLocation();
			float DistancePenalty = FMath::Exp(-(Distance - DeltaImpulse.Size()) / Distance);
			AddForce(Actor, DeltaImpulse * Strength * DistancePenalty, FVector(0, 0, 0), true);
		}
	}
}

void ACubeAbilityRadialMagnetic::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

float ACubeAbilityRadialMagnetic::LerpByCharge(float Min, float Max)
{
	return FMath::Lerp(Min, Max, ChargeTime);
}
