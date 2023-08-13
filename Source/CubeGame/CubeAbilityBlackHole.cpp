// Fill out your copyright notice in the Description page of Project Settings.

#include "CubeAbilityBlackHole.h"
#include "CubeGameCharacter.h"
#include "Kismet/KismetSystemLibrary.h"


ACubeAbilityBlackHole::ACubeAbilityBlackHole()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere");
	
}

void ACubeAbilityBlackHole::BeginPlay()
{
	Super::BeginPlay();

	if (CubeGameCharacter)
	{
		FVector CameraLocation = CubeGameCharacter->GetFollowCamera()->GetComponentLocation();
		FVector CameraForwardVector = CubeGameCharacter->GetFollowCamera()->GetForwardVector();
		SetActorLocation(SpawnDistance * CameraForwardVector + CameraLocation);
		SphereComponent->SetSphereRadius(MinRadius);
		SphereComponent->SetHiddenInGame(true);
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ACubeAbilityBlackHole::OnBeginOverlapping);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ACubeAbilityBlackHole::OnEndOverlapping);
	}
}

void ACubeAbilityBlackHole::PullTowardsCenter()
{
	float Radius = SphereComponent->GetScaledSphereRadius();
	for (auto& Actor: OverlappingTargets)
	{
		if (IsValid(Actor))
		{
			if (GetDistanceTo(Actor) < Radius)
			{
				const FVector Direction = (GetActorLocation() - Actor->GetActorLocation()).GetSafeNormal();
				const float StrengthMultiplier = FMath::Pow(1 - GetDistanceTo(Actor) / Radius, StrengthExponent);
				const FVector Force = Direction * Strength * StrengthMultiplier;
				UKismetSystemLibrary::PrintString(this, Force.ToString());
				AddForce(Actor, Force, FVector(0,0,0), true);
			}
		}
	}
}

void ACubeAbilityBlackHole::OnBeginOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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

void ACubeAbilityBlackHole::OnEndOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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

void ACubeAbilityBlackHole::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentTime = FMath::Clamp(CurrentTime + DeltaSeconds, 0, MinMaxTime);
	SphereComponent->SetSphereRadius(FMath::Lerp(MinRadius, MaxRadius, CurrentTime/MinMaxTime), true);
	
	PullTowardsCenter();
}
