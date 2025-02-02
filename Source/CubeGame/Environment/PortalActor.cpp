// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalActor.h"
#include "Portal.h"
#include "Components/ShapeComponent.h"
#include "CubeGame/CubePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
APortalActor::APortalActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh0");
	RootComponent = MainMesh;
	
}

// Called when the game starts or when spawned
void APortalActor::BeginPlay()
{
	Super::BeginPlay();

	GetComponents(ActorMeshes);
	GetComponents(ShapeComponents);
	for (auto& StaticMesh: ActorMeshes)
	{
		InitStaticMesh(StaticMesh);
	}
	for (auto& ShapeComponent: ShapeComponents)
	{
		InitStaticShape(ShapeComponent);
	}
}

bool APortalActor::GetVisibility(bool bIsVisible)
{
	bIsVisible = bThroughPortal | bIsInversionVisibility? !bIsVisible: bIsVisible;
	bIsVisible = BelongPortal->bIsPlayerSide && bIsInversionVisibility? !bIsVisible: bIsVisible;
	bIsVisible = bIsInversed? !bIsVisible: bIsVisible;
	return bIsVisible;
}

// Called every frame
void APortalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortalActor::Initialize(APortal* InPortal)
{
	BelongPortal = InPortal;
}

void APortalActor::InitStaticMesh(UStaticMeshComponent* StaticMeshComponent)
{
	if (IsValid(StaticMeshComponent))
	{
		StaticMeshComponent->SetScalarParameterValueOnMaterials("Visible", bIsInversed);
		if (bIsInversed | (Tags.Num() == 0))
		{
			// StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			StaticMeshComponent->SetCollisionProfileName(FName("PhysicsActor"));
		}
		else
		{
			// StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			StaticMeshComponent->SetCollisionProfileName(FName("MeshNoCube" + Tags[0].ToString()));
		}
		bVisibility = bIsInversed;
	}
}

void APortalActor::InitStaticShape(UShapeComponent* StaticShapeComponent)
{
	if (IsValid(StaticShapeComponent))
	{
		bool bIsPortalRelated = false;
		for (auto& Tag: Tags)
		{
			if (Tag.ToString().Contains("Portal"))
			{
				bIsPortalRelated = true;
				break;
			}
		}
		if (bIsInversed | !bIsPortalRelated)
		{
			// StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			StaticShapeComponent->SetCollisionProfileName(FName("OverlapAll"));
		}
		else
		{
			// StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			StaticShapeComponent->SetCollisionProfileName(FName("CollisionNoCube"));
		}
	}
}

void APortalActor::Transition(bool bIsVisible)
{
	bThroughPortal = bIsVisible;
	// bVisibility = !bVisibility;
}

void APortalActor::TransitActor()
{
	if (!bTransition)
	{
		bIsInversed = !bIsInversed;
		for (auto& StaticMesh: ActorMeshes)
		{
			if (IsValid(StaticMesh))
			{
				StaticMesh->SetScalarParameterValueOnMaterials("Visible", bIsInversed);
			}
		}
	}
}

void APortalActor::Seen_Implementation(bool bIsVisible)
{
	if (!bTransition)
	{
		bVisibility = GetVisibility(bIsVisible);
		// ECollisionEnabled::Type CollisionType = bVisibility ? ECollisionEnabled::QueryAndPhysics: ECollisionEnabled::QueryAndPhysics;
		FName MeshProfileName;
		if (bVisibility && (bIsInversed? bIsInversionVisibility: !bIsInversionVisibility))
		{
			MeshProfileName = FName("PhysicsActor" + Tags[0].ToString());
		}
		else if (bVisibility)
		{
			MeshProfileName = FName("PhysicsActor");
		}
		else
		{
			MeshProfileName = FName("MeshNoCube" + Tags[0].ToString());
		}
		FName CollisionProfileName = bVisibility ? FName("OverlapAll"): FName("CollisionNoCube");
		for (auto& StaticMesh: ActorMeshes)
		{
			if (IsValid(StaticMesh))
			{
				// StaticMesh->SetCollisionEnabled(CollisionType);
				StaticMesh->SetCollisionProfileName(MeshProfileName);
			}	
		}

		//TODO Debug for WindField & GravityVolume
		for (auto& ShapeComponent: ShapeComponents)
		{
			// ShapeComponent->SetCollisionEnabled(CollisionType);
			ShapeComponent->SetCollisionProfileName(CollisionProfileName);
		}
	}
}

void APortalActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, APortal* Portal)
{
	bool bValidOverlap;
	if (UShapeComponent* ShapeComponent = Cast<UShapeComponent>(GetComponentByClass(UShapeComponent::StaticClass())))
	{
		bValidOverlap = ShapeComponent == OverlappedComponent;
	}
	else
	{
		bValidOverlap = MainMesh == OverlappedComponent;
	}
	
	if (bValidOverlap)
	{
		for (auto& StaticMesh: ActorMeshes)
		{
			if (IsValid(StaticMesh))
			{
				StaticMesh->SetScalarParameterValueOnMaterials("Transition", bVisibility);
			}	
		}
		bTransition = true;

		FVector Front = MainMesh->GetComponentLocation() - Portal->GetPortalPlane()->GetSocketLocation("F");
		FVector Back = 	MainMesh->GetComponentLocation() - Portal->GetPortalPlane()->GetSocketLocation("B");
		bIsBackSide = Front.Size() > Back.Size();
	}
}

void APortalActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, APortal* Portal)
{
	bool bValidOverlap;
	if (UShapeComponent* ShapeComponent = Cast<UShapeComponent>(GetComponentByClass(UShapeComponent::StaticClass())))
	{
		bValidOverlap = ShapeComponent == OverlappedComponent;
	}
	else
	{
		bValidOverlap = MainMesh == OverlappedComponent;
	}
	if (bValidOverlap)
	{
		FVector Front = MainMesh->GetComponentLocation() - Portal->GetPortalPlane()->GetSocketLocation("F");
		FVector Back = 	MainMesh->GetComponentLocation() - Portal->GetPortalPlane()->GetSocketLocation("B");
		if (bIsBackSide != (Front.Size() > Back.Size()))
		{
			bIsInversionVisibility = !bIsInversionVisibility;
		}
		for (auto& StaticMesh: ActorMeshes)
        {
        	if (IsValid(StaticMesh))
        	{
        		StaticMesh->SetScalarParameterValueOnMaterials("Visible", bIsInversed? !bIsInversionVisibility: bIsInversionVisibility);
        	}	
        }
		for (auto& StaticMesh: ActorMeshes)
		{
			if (IsValid(StaticMesh))
			{
				StaticMesh->SetScalarParameterValueOnMaterials("Transition", 0.5);
			}	
		}
		bTransition = false;
		Portal->ResetSeeActor(this);
	}
}

bool APortalActor::IsForceValid(const APortalActor* PortalActor) const
{
	bool bIsForceMerge = false;
	bool bIsActorMerge = false;
	bool ForceSide = bIsInversed? bIsInversionVisibility: !bIsInversionVisibility;
	bool ActorSide = PortalActor->bIsInversed? PortalActor->bIsInversionVisibility: !PortalActor->bIsInversionVisibility;
	TSet<int> MergePortals;
	if (ACubePlayerState* CubePlayerState = Cast<ACubePlayerState>(UGameplayStatics::GetPlayerState(this, 0)))
	{
		MergePortals = CubePlayerState->GetInPortals();
	}
	TArray<FName> OtherTags = PortalActor->Tags;
	// Zero World Force
	if (Tags.Num() == 0)
	{
		return true;
	}
	else
	{
		for (auto& Tag : Tags)
		{
			TArray<FString> ParsedName;
			Tag.ToString().ParseIntoArray(ParsedName, TEXT("Portal"), true);
			if (MergePortals.Contains(UKismetStringLibrary::Conv_StringToInt(ParsedName.Top())))
			{
				bIsForceMerge = true;
			}
		}
	}
	// Zero Actor
	if (OtherTags.Num() == 0)
	{
		if (bIsForceMerge)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		for (auto& Tag: OtherTags)
		{
			TArray<FString> ParsedName;
			Tag.ToString().ParseIntoArray(ParsedName, TEXT("Portal"), true);
			if (MergePortals.Contains(UKismetStringLibrary::Conv_StringToInt(ParsedName.Top())))
			{
				bIsActorMerge = true;
			}
			// Actor and Force in the Same World
			if (Tags.Contains(Tag))
			{
				return ForceSide == ActorSide;
			}
		}
	}

	// Actor and Force in Merge World
	if (bIsForceMerge && bIsActorMerge)
	{
		return ForceSide == ActorSide;
	}

	return false;
}

bool APortalActor::IsActorValid(const AActor* Actor)
{
	if (IsValid(Actor))
	{
		if (const USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Actor->GetComponentByClass<USkeletalMeshComponent>()))
		{
			return SkeletalMeshComponent->IsSimulatingPhysics();
		}
		if (const UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Actor->GetComponentByClass<UStaticMeshComponent>()))
		{
			return StaticMeshComponent->IsSimulatingPhysics();
		}
	}
	return false;
}

