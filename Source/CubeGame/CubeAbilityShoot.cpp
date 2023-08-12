// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeAbilityShoot.h"

#include "Kismet/KismetSystemLibrary.h"

ACubeAbilityShoot::ACubeAbilityShoot()
{
	
}

void ACubeAbilityShoot::FindGrabTarget()
{
	if (CubeGameCharacter)
	{
		const FVector CameraLocation = CubeGameCharacter->GetFollowCamera()->GetComponentLocation();
		const FVector CameraForwardVector = CubeGameCharacter->GetFollowCamera()->GetForwardVector();
		const FVector Start = CameraLocation + GrabRadius * CameraForwardVector;
		const FVector End = CameraLocation + GrabDistance * CameraForwardVector;
		TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes({
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery2),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery3),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery4)}
			);
		FHitResult HitResult;
		TArray<AActor* > IgnoreObjects = GrabTargets;
		IgnoreObjects.Add(CubeGameCharacter);
		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, Start, End, GrabRadius, ObjectTypes,
			false, IgnoreObjects, EDrawDebugTrace::None, HitResult, true))
		{
			AActor* Actor = HitResult.GetActor();
			if (IsValidTarget(Actor))
			{
				AddTarget(Actor);
			}
		}
	}
}

void ACubeAbilityShoot::ShootGrabTarget()
{
	if (CubeGameCharacter)
	{
		if (GrabTargets.Num() > 0)
		{
			TArray<AActor*> IgnoreObjects = GrabTargets;
			IgnoreObjects.Add(CubeGameCharacter);
			AActor* Actor = GrabTargets[0];
			RemoveTarget(Actor);

			const FVector CameraLocation = CubeGameCharacter->GetFollowCamera()->GetComponentLocation();
			const FVector CameraForwardVector = CubeGameCharacter->GetFollowCamera()->GetForwardVector();
			const FVector Start = CameraLocation + ShootRadius * CameraForwardVector;
			const FVector End = CameraLocation + ShootDistance * CameraForwardVector;

			TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes({
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery2),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery3),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery4)}
			);
			FHitResult HitResult;

			FVector Target;
			if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, Start, End, ShootRadius, ObjectTypes,
			false, IgnoreObjects, EDrawDebugTrace::None, HitResult, true))
			{
				Target = HitResult.GetActor()->GetActorLocation();
			}
			else
			{
				if (UKismetSystemLibrary::SphereTraceSingle(this, HitResult.TraceStart, HitResult.TraceEnd, ShootRadius,
					ETraceTypeQuery::TraceTypeQuery1, false, IgnoreObjects, EDrawDebugTrace::None, HitResult, true))
				{
					Target = HitResult.Location;
				}
				else
				{
					Target = HitResult.TraceEnd;	
				}
			}

			AddImpulseToward(Actor, Target, ShootStiffness, ShootDamper);
		}	
	}
}

void ACubeAbilityShoot::BeginPlay()
{
	Super::BeginPlay();
}

void ACubeAbilityShoot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SuctionTarget();
}

void ACubeAbilityShoot::Initialize(ACubeGameCharacter* Character)
{
	Super::Initialize(Character);
}

void ACubeAbilityShoot::AddTarget(AActor* Actor)
{
	if (GrabTargets.Num() < MaxTargets)
	{
		GrabTargets.Add(Actor);
	}
}

void ACubeAbilityShoot::RemoveTarget(AActor* Actor)
{
	if (GrabTargets.Num() > 0 && GrabTargets.Contains(Actor))
	{
		GrabTargets.Remove(Actor);
	}
}

void ACubeAbilityShoot::SuctionTarget()
{
	if (CubeGameCharacter)
	{
		for (int i = 0; i < GrabTargets.Num(); i++)
		{
			AActor* Actor = GrabTargets[i];
			if (IsValid(Actor))
			{
				const float Radian = i / GrabTargets.Num() * 2 * UE_PI;
				const FVector CameraForwardVector = CubeGameCharacter->GetFollowCamera()->GetForwardVector();
				FRotator Rotation = FRotator(0.0, CameraForwardVector.Rotation().Yaw, 0.0);
				FVector Center = FVector(FMath::Cos(Radian) * HoldXY, FMath::Sin(Radian) * HoldXY, HoldZ);
				//TODO Debug
				const FVector Target = Rotation.RotateVector(Center) + CubeGameCharacter->GetMesh()->GetComponentLocation() + CubeGameCharacter->GetMesh()->GetPhysicsLinearVelocity();
				AddForceToward(Actor, Target, 10, 0.1f);
			}
		}
	}
}
