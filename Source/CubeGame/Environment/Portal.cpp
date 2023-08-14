// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"

#include "PortalActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PortalPlane = CreateDefaultSubobject<UStaticMeshComponent>("PortalPlane");
	RootComponent = PortalPlane;

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>("PortalMesh");
	PortalMesh->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("Box");
	BoxComponent->SetupAttachment(RootComponent);
	
	TArray<AActor*> Portals;
	UGameplayStatics::GetAllActorsOfClass(this, PortalClass, Portals);
	for (auto& Actor: Portals)
	{
		if (APortal* Portal = Cast<APortal>(Actor))
		{
			if (Portal != this && Portal->PortalTag == PortalTag)
			{
				OtherPortals.Add(Portal);
			}
		}
	}
	if (bIsActive)
	{
		for (auto& Portal: OtherPortals)
		{
			Portal->bIsActive = false;
		}
		// TODO copy foliage
	}
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APortal::BeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &APortal::EndOverlap);
	
	if (bIsActive)
	{
		PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		PortalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// fix shadows
	UKismetSystemLibrary::ExecuteConsoleCommand(this, "r.Shadow.Virtual.Enable 0");
	UGameplayStatics::GetAllActorsWithTag(this, FName(FString("Portal") + UKismetStringLibrary::Conv_IntToString(PortalTag)), PortalActors);

	for (auto& Actor: PortalActors)
	{
		if (APortalActor* PortalActor = Cast<APortalActor>(Actor))
		{
			PortalActor->Initialize(this);
		}
	}

	bool bCanEnter = true;
	for (auto& Portal: OtherPortals)
	{
		if (Portal->bIsActive)
		{
			bCanEnter = false;
		}
	}

	//TODO debug
	if (bCanEnter && !bIsActive)
	{
		if (PortalTag <= PortalCollections.Num())
		{
			UKismetMaterialLibrary::SetScalarParameterValue(this, PortalCollections[PortalTag - 1], FName("Visibility"), bIsActive);
		}
	}
}

void APortal::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCanItemsOverlap)
	{
		if (APortalActor* PortalActor = Cast<APortalActor>(OtherActor))
		{
			PortalActor->BeginOverlap(OtherComp, this);
		}
	}
}

void APortal::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (bCanItemsOverlap)
	{
		if (APortalActor* PortalActor = Cast<APortalActor>(OtherActor))
		{
			PortalActor->EndOverlap(OtherComp, this);
		}
	}
}

void APortal::ResetSeeActor(AActor* Actor)
{
	if (IsValid(Actor))
	{
		VisibleActors.Remove(Actor);
		NonVisibleActors.Remove(Actor);
	}
	else
	{
		VisibleActors.Empty();
		NonVisibleActors.Empty();
	}
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		const FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetTransformComponent()->GetComponentLocation();
		const FVector PlaneFront = PortalPlane->GetSocketLocation(FName("F"));
		const FVector PlaneBack = PortalPlane->GetSocketLocation(FName("B"));
		const FVector PlaneUp = PortalPlane->GetSocketLocation(FName("U"));
		const FVector PlaneBottom = PortalPlane->GetSocketLocation(FName("D"));
		const FVector PlaneLeft = PortalPlane->GetSocketLocation(FName("L"));
		const FVector PlaneRight = PortalPlane->GetSocketLocation(FName("R"));
		const FVector PortalUpVector = PortalPlane->GetForwardVector();
		const FVector PortalRightVector = PortalPlane->GetRightVector();
		const FVector PortalForwardVector = PortalPlane->GetForwardVector();
		const FVector PortalLocation = PortalPlane->GetComponentLocation();
		bIsBackSide = (CameraLocation - PlaneFront).Size() > (CameraLocation - PlaneBack).Size();
		if (bIsBackSide != bIsBackSideLast)
		{
			bIsBackSideLast = bIsBackSide;
			if (BoxComponent->IsOverlappingActor(UGameplayStatics::GetPlayerPawn(this, 0)))
			{
				//TODO Debug
				TransitCharacter();
			}
		}

		//Setup MPC
		if (PortalTag <= PortalCollections.Num())
		{
			UMaterialParameterCollection* MPC = PortalCollections[PortalTag - 1];
			FVector UpDelta = UKismetMathLibrary::GetUpVector(UKismetMathLibrary::MakeRotFromXY(bIsBackSide ? CameraLocation - PlaneUp: PlaneUp - CameraLocation, PortalRightVector));
			FVector UpStart = PlaneUp + UpDelta * PortalRange;
			FVector UpFinish = UpStart + UpDelta;
			UKismetMaterialLibrary::SetVectorParameterValue(this, MPC, FName("UpStart"), FLinearColor(UpStart));
			UKismetMaterialLibrary::SetVectorParameterValue(this, MPC, FName("UpFinish"), FLinearColor(UpFinish));
			FVector BottomDelta = -UKismetMathLibrary::GetUpVector(UKismetMathLibrary::MakeRotFromXY(bIsBackSide ? CameraLocation - PlaneBottom: PlaneBottom - CameraLocation, PortalRightVector));
			FVector BottomStart = BottomDelta * PortalRange + PlaneBottom;
			FVector BottomFinish = BottomStart + BottomDelta;
			UKismetMaterialLibrary::SetVectorParameterValue(this, MPC, FName("BottomStart"), FLinearColor(BottomStart));
			UKismetMaterialLibrary::SetVectorParameterValue(this, MPC, FName("BottomFinish"), FLinearColor(BottomFinish));
			FVector BackStart = PortalForwardVector * PortalRange * (bIsBackSide? 1: -1) + PortalLocation;
			FVector BackFinish = PortalForwardVector * PortalViewDistance * (bIsBackSide? 1: -1) + PortalLocation;
			
			UKismetMaterialLibrary::SetVectorParameterValue(this, MPC, FName("BackStart"), FLinearColor(BackStart));
			UKismetMaterialLibrary::SetVectorParameterValue(this, MPC, FName("BackFinish"), FLinearColor(BackFinish));
			FVector LeftDelta = UKismetMathLibrary::GetRightVector(UKismetMathLibrary::MakeRotFromXZ(PlaneLeft - CameraLocation, PortalUpVector));
			FVector LeftStart = LeftDelta * PortalRange * (bIsBackSide? 1:-1) + PlaneLeft;
			FVector LeftFinish = LeftStart + LeftDelta * (bIsBackSide? 1:-1);
			
			UKismetMaterialLibrary::SetVectorParameterValue(this, MPC, FName("LeftStart"), FLinearColor(LeftStart));
			UKismetMaterialLibrary::SetVectorParameterValue(this, MPC, FName("LeftFinish"), FLinearColor(LeftFinish));

			FVector RightDelta = UKismetMathLibrary::GetRightVector(UKismetMathLibrary::MakeRotFromXZ(PlaneRight - CameraLocation, PortalUpVector));
			FVector RightStart = RightDelta * PortalRange * (bIsBackSide? -1:1) + PlaneRight;
			FVector RightFinish = RightDelta * (bIsBackSide? -1:1) + RightStart;

			UKismetMaterialLibrary::SetVectorParameterValue(this, MPC, FName("RightStart"), FLinearColor(RightStart));
			UKismetMaterialLibrary::SetVectorParameterValue(this, MPC, FName("RightFinish"), FLinearColor(RightFinish));
			
		}

		//Delay
		TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes({
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery1),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery2),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery3),
			TEnumAsByte<EObjectTypeQuery>(ObjectTypeQuery4)}
			);
		for (auto& Actor: PortalActors)
		{
			if (APortalActor* PortalActor = Cast<APortalActor>(Actor))
			{
				TArray<FHitResult> HitResults;
				bool bHit = UKismetSystemLibrary::LineTraceMultiForObjects(this, PortalActor->GetActorLocation(), CameraLocation,
					ObjectTypes, false, TArray<AActor* >(), EDrawDebugTrace::None, HitResults, false);

				if (bHit)
				{
					for (auto& HitResult: HitResults)
					{
						if (HitResult.GetComponent() == BoxComponent)
						{
							bThroughPortal = true;
							if (VisibleActors.Find(PortalActor) < 0)
							{
								PortalActor->Seen(true);
								VisibleActors.AddUnique(PortalActor);
								NonVisibleActors.Remove(PortalActor);
							}
						}
					}
				}
				if (bThroughPortal)
				{
					bThroughPortal = false;
				}
				else
				{
					if (NonVisibleActors.Find(PortalActor) < 0)
					{
						PortalActor->Seen(false);
						NonVisibleActors.AddUnique(PortalActor);
						VisibleActors.Remove(PortalActor);
					}
				}
			}
		}
	}
}

void APortal::Activate()
{
	bIsActive = true;

	for (auto& Portal: OtherPortals)
	{
		Portal->bIsActive = false;
		Portal->ResetSeeActor(nullptr);
	}
	
	ResetSeeActor(nullptr);
	PortalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void APortal::Transition(bool bActivate)
{
	bIsActive = bActivate;
	if (bIsActive)
	{
		PortalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void APortal::TransitActors()
{
	for (auto& Actor: VisibleActors)
	{
		if (APortalActor* PortalActor = Cast<APortalActor>(Actor))
		{
			PortalActor->TransitActor();
		}
	}
}

void APortal::TransitCharacter()
{
	if (PortalTag <= PortalCollections.Num())
	{
		float Visibility = UKismetMaterialLibrary::GetScalarParameterValue(this, PortalCollections[PortalTag - 1], FName("Visibility"));
		Visibility = 1 - Visibility;
		bIsPlayerSide = Visibility == 1.0 ? true: false;
		UKismetMaterialLibrary::SetScalarParameterValue(this, PortalCollections[PortalTag - 1], FName("Visibility"), Visibility);
		for (auto& Actor: PortalActors)
		{
			if (APortalActor* PortalActor = Cast<APortalActor>(Actor))
			{
				PortalActor->Transition(Visibility == 1.0);
			}
		}

		for (auto& Portal: OtherPortals)
		{
			Portal->bIsPlayerSide = bIsPlayerSide;
		}
		// TODO foliage

		ResetSeeActor(nullptr);
	}
}



