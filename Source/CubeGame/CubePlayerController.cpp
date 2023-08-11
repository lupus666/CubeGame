// Fill out your copyright notice in the Description page of Project Settings.


#include "CubePlayerController.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ACubePlayerController::ACubePlayerController()
{
	{
		CapsulePercentageForTrace = 1.0f;
		DebugLineTraces = true;
		IsOcclusionEnabled = true;
	}
}

void ACubePlayerController::SyncOccludedActors()
{
	if (!ShouldCheckCameraOcclusion()) return;
 
	// Camera is currently colliding, show all current occluded actors
	// and do not perform further occlusion
	if (ActiveSpringArm->bDoCollisionTest)
	{
		ForceShowOccludedActors();
		return;
	}
 
	FVector Start = ActiveCamera->GetComponentLocation();
	FVector End = GetPawn()->GetActorLocation();
 
	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectTypes;
	CollisionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
 
	TArray<AActor*> ActorsToIgnore; // TODO: Add configuration to ignore actor types
	TArray<FHitResult> OutHits;
 
	auto ShouldDebug = DebugLineTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
 
	bool bGotHits = UKismetSystemLibrary::CapsuleTraceMultiForObjects(
	  GetWorld(), Start, End, ActiveCapsuleComponent->GetScaledCapsuleRadius() * CapsulePercentageForTrace,
	  ActiveCapsuleComponent->GetScaledCapsuleHalfHeight() * CapsulePercentageForTrace, CollisionObjectTypes, true,
	  ActorsToIgnore,
	  ShouldDebug,
	  OutHits, true);
 
	if (bGotHits)
	{
		// The list of actors hit by the line trace, that means that they are occluded from view
		TSet<const AActor*> ActorsJustOccluded;
 
		// Hide actors that are occluded by the camera
		for (FHitResult Hit : OutHits)
		{
			const AActor* HitActor = Cast<AActor>(Hit.GetActor());
			HideOccludedActor(HitActor);
			ActorsJustOccluded.Add(HitActor);
		}
 
		// Show actors that are currently hidden but that are not occluded by the camera anymore 
		for (auto& Elem : OccludedActors)
		{
			if (!ActorsJustOccluded.Contains(Elem.Value.Actor) && Elem.Value.bIsOccluded)
			{
				ShowOccludedActor(Elem.Value);
 
				if (DebugLineTraces)
				{
					UE_LOG(LogTemp, Warning,
						   TEXT("Actor %s was occluded, but it's not occluded anymore with the new hits."), *Elem.Value.Actor->GetName());
				}
			}
		}
	}
	else
	{
		ForceShowOccludedActors();
	}
}

void ACubePlayerController::BeginPlay()
{
	Super::BeginPlay();
 
	if (IsValid(GetPawn()))
	{
		ActiveSpringArm = Cast<USpringArmComponent>(GetPawn()->GetComponentByClass<USpringArmComponent>());
		ActiveCamera = Cast<UCameraComponent>(GetPawn()->GetComponentByClass<UCameraComponent>());
		ActiveCapsuleComponent = Cast<UCapsuleComponent>(GetPawn()->GetComponentByClass<UCapsuleComponent>());
	}
}

bool ACubePlayerController::HideOccludedActor(const AActor* Actor)
{
	FCameraOccludedActor* ExistingOccludedActor = OccludedActors.Find(Actor);
 
	if (ExistingOccludedActor && ExistingOccludedActor->bIsOccluded)
	{
		if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s was already occluded. Ignoring."), *Actor->GetName());
		return false;
	}
 
	if (ExistingOccludedActor && IsValid(ExistingOccludedActor->Actor))
	{
		ExistingOccludedActor->bIsOccluded = true;
		OnHideOccludedActor(*ExistingOccludedActor);
 
		if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s exists, but was not occluded. Occluding it now."), *Actor->GetName());
	}
	else
	{

		if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass())))
		{
			FCameraOccludedActor OccludedActor;
			OccludedActor.Actor = Actor;
			OccludedActor.StaticMeshComponent = StaticMesh;
			OccludedActor.Materials = StaticMesh->GetMaterials();
			OccludedActor.bIsOccluded = true;
			OccludedActors.Add(Actor, OccludedActor);
			OnHideOccludedActor(OccludedActor);
			if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s does not exist, creating and occluding it now."), *Actor->GetName());
		}
	}
 
	return true;
}

void ACubePlayerController::OnHideOccludedActor(const FCameraOccludedActor& OccludedActor) const
{
	if (OccludedActor.StaticMeshComponent && OccludedActor.Materials.Num() > 0)
	{
		for (int i = 0; i < OccludedActor.StaticMeshComponent->GetNumMaterials(); ++i)
		{
			OccludedActor.StaticMeshComponent->SetMaterial(i, FadeMaterial);
		}
	}
}

void ACubePlayerController::ShowOccludedActor(FCameraOccludedActor& OccludedActor)
{
	if (!IsValid(OccludedActor.Actor))
	{
		OccludedActors.Remove(OccludedActor.Actor);
	}
 
	OccludedActor.bIsOccluded = false;
	OnShowOccludedActor(OccludedActor);
}

void ACubePlayerController::OnShowOccludedActor(const FCameraOccludedActor& OccludedActor) const
{
	if (OccludedActor.StaticMeshComponent && OccludedActor.Materials.Num() > 0)
	{
		for (int matIdx = 0; matIdx < OccludedActor.Materials.Num(); ++matIdx)
		{
			OccludedActor.StaticMeshComponent->SetMaterial(matIdx, OccludedActor.Materials[matIdx]);
		}
	}
	
}

void ACubePlayerController::ForceShowOccludedActors()
{
	for (auto& Elem : OccludedActors)
	{
		if (Elem.Value.bIsOccluded)
		{
			ShowOccludedActor(Elem.Value);
 
			if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s was occluded, force to show again."), *Elem.Value.Actor->GetName());
		}
	}
}
