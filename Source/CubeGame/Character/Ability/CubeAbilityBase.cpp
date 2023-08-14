// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeAbilityBase.h"

#include "CubeGame/Environment/GravityVolumeBase.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "CubeGame/Character/CubeGameCharacter.h"


// Sets default values
ACubeAbilityBase::ACubeAbilityBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACubeAbilityBase::Initialize(ACubeGameCharacter* Character)
{
	this->CubeGameCharacter = Character;
}

// Called when the game starts or when spawned
void ACubeAbilityBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACubeAbilityBase::AddImpulse(AActor* Actor, FVector Impulse, FVector AngularImpulse, bool bIgnoreMass)
{
	if (IsValid(Actor))
	{
		if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Actor->GetComponentByClass<UStaticMeshComponent>()))
		{
			StaticMeshComponent->AddImpulse(Impulse, EName::None, bIgnoreMass);
			StaticMeshComponent->AddAngularImpulseInRadians(AngularImpulse, EName::None, bIgnoreMass);
		}
		else if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Actor->GetComponentByClass<USkeletalMeshComponent>()))
		{
			SkeletalMeshComponent->AddImpulse(Impulse, EName::None, bIgnoreMass);
			SkeletalMeshComponent->AddAngularImpulseInRadians(AngularImpulse, EName::None, bIgnoreMass);
		}
		else
		{
			
		}
	}
}

void ACubeAbilityBase::AddImpulseToward(AActor* Actor, FVector TargetImpulse, float LinearStiffness, float AngularDamper)
{
	if (IsValid(Actor))
	{
		FVector LinearVelocity;
		FVector AngularVelocity;
		GetTargetVelocity(Actor, LinearVelocity, AngularVelocity);
		const FVector DeltaImpulse = TargetImpulse - Actor->GetActorLocation();
		const FVector Impulse = DeltaImpulse.GetSafeNormal() * LinearStiffness - LinearVelocity - GetTargetGravity(Actor) * 0.5 * DeltaImpulse.Size() / LinearStiffness;
		const FVector AngularImpulse = AngularVelocity * -AngularDamper;
		UKismetSystemLibrary::PrintString(this, Impulse.ToString());
		UKismetSystemLibrary::PrintString(this, DeltaImpulse.ToString());

		AddImpulse(Actor, Impulse, AngularImpulse, true);
	}
}

void ACubeAbilityBase::AddForce(AActor* Actor, FVector Force, FVector Torque, bool bIgnoreMass)
{
	if (IsValid(Actor))
	{
		if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Actor->GetComponentByClass<UStaticMeshComponent>()))
		{
			StaticMeshComponent->AddForce(Force, EName::None, bIgnoreMass);
			StaticMeshComponent->AddTorqueInRadians(Torque, EName::None, bIgnoreMass);
		}
		else if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Actor->GetComponentByClass<USkeletalMeshComponent>()))
		{
			SkeletalMeshComponent->AddForce(Force, EName::None, bIgnoreMass);
			SkeletalMeshComponent->AddTorqueInRadians(Torque, EName::None, bIgnoreMass);
		}
		else
		{
			
		}
	}
}

void ACubeAbilityBase::AddForceToward(AActor* Actor, FVector TargetForce, float LinearStiffness, float AngularDamper)
{
	if (IsValid(Actor))
	{
		FVector LinearVelocity;
		FVector AngularVelocity;
		GetTargetVelocity(Actor, LinearVelocity, AngularVelocity);
		const FVector Force = (TargetForce - Actor->GetActorLocation() - LinearVelocity) * LinearStiffness - GetTargetGravity(Actor);;
		const FVector Torque = AngularVelocity * -AngularDamper;
		AddForce(Actor, Force, Torque, true);
	}
}

bool ACubeAbilityBase::IsValidTarget(AActor* Actor)
{
	if (IsValid(Actor))
	{
		if (const UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Actor->GetComponentByClass<UStaticMeshComponent>()))
		{
			return StaticMeshComponent->IsSimulatingPhysics();
		}
		else if (const USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Actor->GetComponentByClass<USkeletalMeshComponent>()))
		{
			return SkeletalMeshComponent->IsSimulatingPhysics();
		}
	}
	return false;
}

TArray<AActor*> ACubeAbilityBase::FilterTargets(TArray<AActor*>& Targets)
{
	TSet<AActor*> FilteredTargets;
	for(auto& Actor: Targets)
	{
		if (IsValid(Actor))
		{
			if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Actor->GetComponentByClass<UStaticMeshComponent>()))
			{
				if (StaticMeshComponent->IsSimulatingPhysics())
				{
					FilteredTargets.Add(Actor);
				}
			}
			else if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Actor->GetComponentByClass<USkeletalMeshComponent>()))
			{
				if (SkeletalMeshComponent->IsSimulatingPhysics())
				{
					FilteredTargets.Add(Actor);
				}
			}
		}
	}
	return TArray<AActor*>(FilteredTargets.Array());
}

TArray<AActor*> ACubeAbilityBase::FilterHitTargets(TArray<FHitResult*>& HitResults)
{
	TSet<AActor*> FilteredTargets;
	for(const auto& HitResult: HitResults)
	{
		if (AActor* Actor = HitResult->GetActor(); IsValid(Actor))
		{
			if (const UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Actor->GetComponentByClass<UStaticMeshComponent>()))
			{
				if (StaticMeshComponent->IsSimulatingPhysics())
				{
					FilteredTargets.Add(Actor);
				}
			}
			else if (const USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Actor->GetComponentByClass<USkeletalMeshComponent>()))
			{
				if (SkeletalMeshComponent->IsSimulatingPhysics())
				{
					FilteredTargets.Add(Actor);
				}
			}
		}
	}
	return TArray<AActor*>(FilteredTargets.Array());
}

void ACubeAbilityBase::GetTargetVelocity(AActor* Actor, FVector& Velocity, FVector& AngularVelocity)
{
	if (IsValid(Actor))
	{
		if (const UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Actor->GetComponentByClass<UStaticMeshComponent>()))
		{
			Velocity = StaticMeshComponent->GetComponentVelocity();
			AngularVelocity = StaticMeshComponent->GetPhysicsAngularVelocityInRadians();
		}
		else if (const USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Actor->GetComponentByClass<USkeletalMeshComponent>()))
		{
			Velocity = SkeletalMeshComponent->GetComponentVelocity();
			AngularVelocity = SkeletalMeshComponent->GetPhysicsAngularVelocityInRadians();
		}
	}
}

FVector ACubeAbilityBase::GetTargetGravity(AActor* Actor)
{
	if (IsValid(Actor))
	{
		UPrimitiveComponent* PrimitiveComponent = Actor->GetComponentByClass<UPrimitiveComponent>();
		TArray<AActor* > OverlappingActors;
		PrimitiveComponent->GetOverlappingActors(OverlappingActors);
		for (auto& OverlappingActor: OverlappingActors)
		{
			if (AGravityVolumeBase* GravityVolumeBase = Cast<AGravityVolumeBase>(OverlappingActor))
			{
				return GravityVolumeBase->GravityDirection * GravityVolumeBase->GravityAccelerate;
			}
		}
	}
	return FVector(0.0, 0.0, 0.0);
}

// Called every frame
void ACubeAbilityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

