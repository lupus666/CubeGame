// Fill out your copyright notice in the Description page of Project Settings.


#include "WindField.h"

#include "CubeAnimInstance.h"
#include "CubeGameCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/PhysicsAsset.h"

// Sets default values
AWindField::AWindField()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create effect to show windfield
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called when the game starts or when spawned
void AWindField::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWindField::ChangeWindDirection()
{
	
}

void AWindField::ApplyWindEffect(AActor* Actor)
{
	if (USkeletalMeshComponent* SkeletalMesh = Actor->GetComponentByClass<USkeletalMeshComponent>())
	{
		UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
		const ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(Actor);

		// Single cube as a rigid body ignore torque
		if (CubeGameCharacter)
		{
			TArray<FName> BoneNames;
			SkeletalMesh->GetBoneNames(BoneNames);
			for (FName& BoneName: BoneNames)
			{
				if (BoneName == "Pelvis" && CubeGameCharacter->GetCurrentRelaxRate() < 1.0f)
				{
					continue;
				}
				if (FBodyInstance* BodyInstance = SkeletalMesh->GetBodyInstance(BoneName))
				{
					// if (BodyInstance->LineTrace())
					{
						float BaseArea;
						if (BodyInstance->GetBodySetup()->AggGeom.BoxElems.Num())
						{
							BaseArea = pow(BodyInstance->GetBodySetup()->AggGeom.BoxElems[0].X*0.01, 2);
						}
						else
						{
							BaseArea = UE_PI*pow(BodyInstance->GetBodySetup()->AggGeom.SphereElems[0].Radius*0.01, 2);
						}
						float TotalArea = 0;
						for (auto& Normal: GetCubeNormal(UKismetMathLibrary::GetForwardVector(BodyInstance->GetUnrealWorldTransform().Rotator())))
						{
							float DotProduct = FVector::DotProduct(WindDirection.Vector(), Normal);
							if (DotProduct > 0.0f)
							{
								continue;
							}
							else
							{
								TotalArea += BaseArea * -DotProduct/(WindDirection.Vector().Size());
							}
						}
						const FVector Force = ComputeWindLoad(TotalArea);
						BodyInstance->AddForce(Force);
					}
				}
				if (BoneName == "Pelvis" && CubeGameCharacter->GetCurrentRelaxRate() >= 1.0f)
				{
					break;
				}
			}
		}
		// Whole cubes as a rigid body
		else
		{
			//TODO shader
		}
	}
	else if (UStaticMeshComponent* StaticMesh = Actor->GetComponentByClass<UStaticMeshComponent>())
	{
		
	}
}

FVector AWindField::ComputeWindLoad(float WindSurfaceArea)
{
	const float rho = 1.204;
	return 0.5* rho * WindSurfaceArea* UKismetMathLibrary::Normal(UKismetMathLibrary::GetForwardVector(WindDirection)) * WindStrength * WindStrength;
}

TArray<FVector> AWindField::GetCubeNormal(const FVector& ForwardVector)
{
	TArray<FVector> CubeNormals;
	const FVector UnitForwardVector = UKismetMathLibrary::Normal(ForwardVector);
	CubeNormals.Add(UnitForwardVector);
	CubeNormals.Add(-UnitForwardVector);
	CubeNormals.Add(FVector::CrossProduct(UnitForwardVector, FVector(0, 0, 1)));
	CubeNormals.Add(-FVector::CrossProduct(UnitForwardVector, FVector(0, 0, 1)));
	CubeNormals.Add(FVector::CrossProduct(UnitForwardVector, FVector(0, 1, 0)));
	CubeNormals.Add(-FVector::CrossProduct(UnitForwardVector, FVector(0, 1, 0)));
	return CubeNormals;
}


// Called every frame
void AWindField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsDirectionalWind)
	{
		
	}


	// Update WindStrength
	if (WindSpeedCurve)
	{
		WindStrength = WindSpeedCurve->GetFloatValue(CurveTime) * StrengthMultiplier;
	}
	else
	{
		WindStrength = WindSpeed * StrengthMultiplier;
	}

	//Apply Force
	for (AActor* Actor: WindFieldActors)
	{
		if (IsValid(Actor))
		{
			ApplyWindEffect(Actor);
		}
	}

	CurveTime += DeltaTime;
}

