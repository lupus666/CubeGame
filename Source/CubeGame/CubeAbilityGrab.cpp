// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeAbilityGrab.h"
#include "CubeGameCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

ACubeAbilityGrab::ACubeAbilityGrab()
{
	
}

void ACubeAbilityGrab::BeginPlay()
{
	Super::BeginPlay();

	FindGrabTarget();
}

void ACubeAbilityGrab::FindGrabTarget()
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
		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, Start, End, GrabRadius, ObjectTypes,
			false, TArray<AActor* >({CubeGameCharacter}), EDrawDebugTrace::None, HitResult, true))
		{
			AActor* Actor = HitResult.GetActor();
			if (IsValidTarget(Actor))
			{
				SetGrabTarget(Actor);
			}
		}
	}
}

void ACubeAbilityGrab::ThrowGrabTarget()
{
	if (CubeGameCharacter)
	{
		if (IsValid(GrabTarget))
		{
			TArray<AActor*> IgnoreObjects({GrabTarget, CubeGameCharacter});

			const FVector CameraLocation = CubeGameCharacter->GetFollowCamera()->GetComponentLocation();
			const FVector CameraForwardVector = CubeGameCharacter->GetFollowCamera()->GetForwardVector();
			const FVector Start = CameraLocation + ThrowRadius * CameraForwardVector;
			const FVector End = CameraLocation + ThrowDistance * CameraForwardVector;

			TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes({
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery2),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery3),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery4)}
			);
			FHitResult HitResult;

			FVector Target;
			if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, Start, End, ThrowRadius, ObjectTypes,
			false, IgnoreObjects, EDrawDebugTrace::None, HitResult, true))
			{
				Target = HitResult.GetActor()->GetActorLocation();
			}
			else
			{
				if (UKismetSystemLibrary::SphereTraceSingle(this, HitResult.TraceStart, HitResult.TraceEnd, ThrowRadius,
					ETraceTypeQuery::TraceTypeQuery1, false, IgnoreObjects, EDrawDebugTrace::None, HitResult, true))
				{
					Target = HitResult.Location;
				}
				else
				{
					Target = HitResult.TraceEnd;	
				}
			}

			AddImpulseToward(GrabTarget, Target, ThrowStiffness, ThrowDamper);
			ReleaseGrabTarget();
		}
	}
}

void ACubeAbilityGrab::ReleaseGrabTarget()
{
	SetGrabTarget(nullptr);
}

AActor* ACubeAbilityGrab::GetGrabTarget() const
{
	return GrabTarget;
}

void ACubeAbilityGrab::SetGrabTarget(AActor* Target)
{
	this->GrabTarget = Target;
}

void ACubeAbilityGrab::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CubeGameCharacter)
	{
		if (IsValid(GrabTarget))
		{
			const FVector CameraLocation = CubeGameCharacter->GetFollowCamera()->GetComponentLocation();
			const FVector CameraForwardVector = CubeGameCharacter->GetFollowCamera()->GetForwardVector();
			FVector Target = CameraLocation + CameraForwardVector * HoldDistance;
			if (bConsiderVelocity)
			{
				Target += CubeGameCharacter->GetMesh()->GetPhysicsLinearVelocity();
			}
			AddForceToward(GrabTarget, Target);
		}
	}
}

