// Fill out your copyright notice in the Description page of Project Settings.

#include "CubeAbilityDilationDefense.h"
#include "CubeGameCharacter.h"


ACubeAbilityDilationDefense::ACubeAbilityDilationDefense()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
}

void ACubeAbilityDilationDefense::BeginPlay()
{
	Super::BeginPlay();
	DefenseAngleCos = FMath::Cos(FMath::DegreesToRadians(DefenseAngleDegree/2));
	if (CubeGameCharacter)
	{
		SetActorLocation(CubeGameCharacter->GetMesh()->GetComponentLocation());
		SphereComponent->SetSphereRadius(DefenseRadius, true);
		SphereComponent->SetHiddenInGame(true);
	}
}

void ACubeAbilityDilationDefense::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DilationDefense();
}


void ACubeAbilityDilationDefense::DilationDefense()
{
	if (CubeGameCharacter)
	{
		SetActorLocation(CubeGameCharacter->GetMesh()->GetComponentLocation());
		for (auto& Actor: OverlappingTargets)
		{
			if (IsValid(Actor))
			{
				float DotProduct = FVector::DotProduct(
					CubeGameCharacter->GetFollowCamera()->GetForwardVector() * FVector(1.0, 1.0, 0.0).GetSafeNormal(),
					((Actor->GetActorLocation() - CubeGameCharacter->GetMesh()->GetComponentLocation()) * FVector(1.0, 1.0, 0.0)).GetSafeNormal()
					);
				if (DotProduct >= DefenseAngleCos)
				{
					FVector LinearVelocity;
					FVector AngularVelocity;
					GetTargetVelocity(Actor, LinearVelocity, AngularVelocity);

					AddImpulse(Actor, -DilationRatio * LinearVelocity, -DilationRatio * AngularVelocity, false);
					AddForce(Actor, -GetTargetGravity(Actor), FVector(0.0, 0.0, 0.0), true);
				}
			}
		}
	}
}

void ACubeAbilityDilationDefense::OnBeginOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CubeGameCharacter != Cast<ACubeGameCharacter>(OtherActor))
	{
		if (IsValidTarget(OtherActor))
		{
			OverlappingTargets.Add(OtherActor);
		}
	}
}

void ACubeAbilityDilationDefense::OnEndOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValidTarget(OtherActor))
	{
		if (OverlappingTargets.Contains(OtherActor))
		{
			OverlappingTargets.Remove(OtherActor);
		}
	}
}
