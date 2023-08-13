// Fill out your copyright notice in the Description page of Project Settings.


#include "WindField.h"

#include "CubeGameCharacter.h"
#include "WindComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
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
	SceneCaptureDepth = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureDepth");
	SceneCaptureDepth->SetupAttachment(RootComponent);
	SceneCaptureDepth->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureDepth->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCaptureDepth->CaptureSource = ESceneCaptureSource::SCS_DeviceDepth;
	SceneCaptureDepth->PostProcessBlendWeight = 0.0;

	SceneCaptureNormal = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureNormal");
	SceneCaptureNormal->SetupAttachment(RootComponent);
	SceneCaptureNormal->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureNormal->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCaptureNormal->CaptureSource = ESceneCaptureSource::SCS_Normal;
	SceneCaptureNormal->PostProcessBlendWeight = 0.0;
}

// Called when the game starts or when spawned
void AWindField::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWindField::ChangeWindDirection()
{
	
}

void AWindField::AddWindLoad(AActor* Actor)
{
	if (USkeletalMeshComponent* SkeletalMesh = Actor->GetComponentByClass<USkeletalMeshComponent>())
	{
		UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
		const ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(Actor);

		// Single cube as a rigid body ignore torque
		if (CubeGameCharacter && CubeGameCharacter->GetCurrentRelaxRate() < 1.0f)
		{
			TArray<FName> BoneNames;
			SkeletalMesh->GetBoneNames(BoneNames);
			for (FName& BoneName: BoneNames)
			{
				if (FBodyInstance* BodyInstance = SkeletalMesh->GetBodyInstance(BoneName))
				{
				
					// const float SurfaceArea = CalcWindLoadByBodyInstance(BodyInstance);
					// const FVector Force = CalcWindLoadByArea(SurfaceArea);
					const FVector Force = CalcWindForceByBodyInstance(BodyInstance);
					BodyInstance->AddForce(Force);
				}
			}
		}
		// Whole cubes as a rigid body
		else
		{
			// Get Cube BoundingBox
			const FBox Bounds = CubeGameCharacter->GetMesh()->Bounds.GetBox();
			constexpr float CaptureDistance = 80.0f;
			const FVector GravityCenter = CubeGameCharacter->GetMesh()->GetBoneLocation(CubeGameCharacter->GetBodyName());
			const FVector CaptureLocation = GravityCenter - UKismetMathLibrary::Normal(WindDirection) * CaptureDistance;
			
			UTextureRenderTarget2D* RTDepthMap = UKismetRenderingLibrary::CreateRenderTarget2D(this, 256, 256, ETextureRenderTargetFormat::RTF_RGBA16f);
			SceneCaptureDepth->ShowOnlyActors.Empty();
			SceneCaptureDepth->ShowOnlyActors.Add(Actor);
			SceneCaptureDepth->TextureTarget = RTDepthMap;
			SceneCaptureDepth->OrthoWidth = UKismetMathLibrary::Max(Bounds.GetSize().X*1.732, Bounds.GetSize().Y*1.732);
			SceneCaptureDepth->SetWorldLocation(CaptureLocation);
			SceneCaptureDepth->SetWorldRotation(WindDirection.Rotation());
			SceneCaptureDepth->CaptureScene();

			UTextureRenderTarget2D* RTNormalMap = UKismetRenderingLibrary::CreateRenderTarget2D(this, 256, 256, ETextureRenderTargetFormat::RTF_RGBA16f);
			SceneCaptureNormal->ShowOnlyActors.Empty();
			SceneCaptureNormal->ShowOnlyActors.Add(Actor);
			SceneCaptureNormal->TextureTarget = RTNormalMap;
			SceneCaptureNormal->OrthoWidth = UKismetMathLibrary::Max(Bounds.GetSize().X*1.732, Bounds.GetSize().Y*1.732);
			SceneCaptureNormal->SetWorldLocation(CaptureLocation);
			SceneCaptureNormal->SetWorldRotation(WindDirection.Rotation());
			SceneCaptureNormal->CaptureScene();
			
			//TODO performance optimization
			FRenderTarget* RenderTargetDepth = RTDepthMap->GameThread_GetRenderTargetResource();
			TArray<FColor> PixelsDepth;
			FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
			RenderTargetDepth->ReadPixels(PixelsDepth, ReadPixelFlags);

			FRenderTarget* RenderTargetNormal = RTDepthMap->GameThread_GetRenderTargetResource();
			TArray<FColor> PixelsNormal;
			RenderTargetNormal->ReadPixels(PixelsNormal, ReadPixelFlags);


			const FIntPoint TextureSize = RenderTargetDepth->GetSizeXY();
			
			// FMatrix ViewRotationMatrix = FLookAtMatrix(CaptureLocation, GravityCenter, UKismetMathLibrary::GetUpVector(WindDirection));
			// FMatrix ProjectionMatrix = FReversedZOrthoMatrix(SceneCaptureComponent2D->OrthoWidth/2.0f,SceneCaptureComponent2D->OrthoWidth/2.0f, 2.0f/WORLD_MAX, WORLD_MAX);
			{
				// FMatrix ViewProjectionMatrix = ViewRotationMatrix * ProjectionMatrix;
				// FVector4 CameraPosition = ViewProjectionMatrix.TransformFVector4(FVector4(GravityCenter.X, GravityCenter.Y, GravityCenter.Z, 1.0f));
				// FVector4 ScreenPosition = CameraPosition/CameraPosition.W;
				// ScreenPosition.X = ScreenPosition.X / 2 + 0.5f;
				// ScreenPosition.Y = ScreenPosition.Y / 2 + 0.5f;
				// FIntPoint PixelPosition(FMath::RoundToInt(ScreenPosition.X * TextureSize.X), FMath::RoundToInt(ScreenPosition.Y * TextureSize.Y));
				// UKismetSystemLibrary::PrintString(this, PixelPosition.ToString());
			}
			float PixelX = SceneCaptureDepth->OrthoWidth / TextureSize.X * 0.01; 
			float PixelY = SceneCaptureDepth->OrthoWidth / TextureSize.Y * 0.01;
			float PixelArea = PixelX * PixelY;

			int PixelCount = 0;
			FVector TotalR = FVector(0, 0, 0);
			
			for (int i = 0; i < TextureSize.X; i++)
			{
				for (int j = 0; j < TextureSize.Y; j++)
				{
					if (PixelsDepth[i * TextureSize.Y + j] != FColor::Black)
					{
						PixelCount += 1;
						TotalR += FVector(i + 0.5 - TextureSize.X/2.0f, j + 0.5 - TextureSize.Y/2.0f, 0);
					}
				}
			}
			
			{
			// 	FVector PixelPosition = TotalR + FVector(RenderTarget->GetSizeXY().X, RenderTarget->GetSizeXY().Y, 0.0f);
			// 	PixelPosition = FVector(256, 256, 0);
			// 	FVector2D ScreenPosition(PixelPosition.X/TextureSize.X, PixelPosition.Y/TextureSize.Y);
			// 	FVector4 ClipPosition(ScreenPosition.X*2.0f-1.0f, ScreenPosition.Y*2.0f-1.0f, CaptureDistance, 1.0f);
			// 	FMatrix InverseViewProjectionMatrix = ProjectionMatrix.Inverse() * ViewRotationMatrix.Inverse();
			// 	FVector4 WorldPosition = InverseViewProjectionMatrix.TransformFVector4(ClipPosition);
			// 	UKismetSystemLibrary::PrintString(this, WorldPosition.ToString());
			}

			//TODO Total lever arm BUG
			
			FVector RightVector = SceneCaptureDepth->GetRightVector();
			FVector UpVector = SceneCaptureDepth->GetUpVector();
			FVector RVector = -TotalR.X * PixelX * UpVector + TotalR.Y * PixelY * RightVector;
			const float SurfaceArea = PixelCount * PixelArea;

			//TODO fix
			const FVector TotalForce = CalcWindLoadByArea(SurfaceArea);

			//TODO deduce
			const FVector TotalTorque = UKismetMathLibrary::Cross_VectorVector(RVector, TotalForce);
			CubeGameCharacter->GetMesh()->AddForce(TotalForce);
			CubeGameCharacter->GetMesh()->AddTorqueInRadians(TotalTorque);

			// UKismetSystemLibrary::PrintString(this, TextureSize.ToString());
			// UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_IntToString(PixelCount));
			// UKismetSystemLibrary::PrintString(this, TotalR.ToString());
			// UKismetSystemLibrary::PrintString(this, (GravityCenter+RVector).ToString());
			// UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(SurfaceArea));
			// UKismetSystemLibrary::PrintString(this, TotalForce.ToString());
			// UKismetSystemLibrary::PrintString(this, TotalTorque.ToString());
		}
	}
	else if (UStaticMeshComponent* StaticMesh = Actor->GetComponentByClass<UStaticMeshComponent>())
	{
		if (FBodyInstance* BodyInstance = StaticMesh->GetBodyInstance())
		{
			FVector Force;
			//TODO GetMeshTriangleArea
			
			Force = CalcWindForceByBodyInstance(BodyInstance);
			if (Force == FVector(0, 0, 0))
			{
				if (UWindComponent* WindComponent = Actor->GetComponentByClass<UWindComponent>())
				{
					if (WindComponent->StaticSurfaceArea != 0.0)
					{
						Force = CalcWindLoadByArea(WindComponent->StaticSurfaceArea) * WindDirection;
					}
				}
			}
			BodyInstance->AddForce(Force);
		}
	}
}

void AWindField::CaptureDepthNormal()
{

}

float AWindField::CalcWindLoadByArea(float WindSurfaceArea) const
{
	const float rho = 1.204;
	return 0.5 * rho * WindSurfaceArea * WindStrength * WindStrength;
}

TArray<FVector> AWindField::GetCubeNormals(const FVector& ForwardVector)
{
	TArray<FVector> CubeNormals;
	const FVector UnitForwardVector = ForwardVector.GetSafeNormal();
	CubeNormals.Add(UnitForwardVector);
	CubeNormals.Add(-UnitForwardVector);
	CubeNormals.Add(FVector::CrossProduct(UnitForwardVector, FVector(0, 0, 1)).GetSafeNormal());
	CubeNormals.Add(-FVector::CrossProduct(UnitForwardVector, FVector(0, 0, 1)).GetSafeNormal());
	CubeNormals.Add(FVector::CrossProduct(UnitForwardVector, FVector(0, 1, 0)).GetSafeNormal());
	CubeNormals.Add(-FVector::CrossProduct(UnitForwardVector, FVector(0, 1, 0)).GetSafeNormal());
	return CubeNormals;
}

FVector AWindField::CalcWindForceByBodyInstance(FBodyInstance* BodyInstance) const
{
	if (BodyInstance->GetBodySetup()->AggGeom.BoxElems.Num())
	{
		float BaseArea = pow(BodyInstance->GetBodySetup()->AggGeom.BoxElems[0].X*0.01, 2);
		FVector Force = FVector(0, 0, 0);
		for (auto& Normal: GetCubeNormals(UKismetMathLibrary::GetForwardVector(BodyInstance->GetUnrealWorldTransform().Rotator())))
		{
			float DotProduct = FVector::DotProduct(WindDirection.GetSafeNormal(), Normal);
			if (DotProduct < 0.0f)
			{
				Force += CalcWindLoadByArea(BaseArea * -DotProduct) * -DotProduct * -Normal;
				// TotalArea += BaseArea * -DotProduct/(WindDirection.Size());
			}
				
		}
		return Force;
	}
	else if(BodyInstance->GetBodySetup()->AggGeom.SphereElems.Num())
	{
		return CalcWindLoadByArea(UE_PI *pow(BodyInstance->GetBodySetup()->AggGeom.SphereElems[0].Radius*0.01, 2)) * WindDirection.GetSafeNormal();
	}
	else
	{
		
		return FVector(0, 0, 0);
	}
	
	
}

FVector AWindField::CalcWindForceByRenderTarget()
{

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

	if (BoxComponent)
	{
		TArray<AActor*> OverlappingActors;
		BoxComponent->GetOverlappingActors(OverlappingActors);
		for (auto& Actor : OverlappingActors)
		{
			if (IsValid(Actor))
			{
				if (UWindComponent* WindComponent = Cast<UWindComponent>(Actor->GetComponentByClass<UWindComponent>()))
				{
					AddWindLoad(Actor);
				}
			}
		}
	}

	CurveTime += DeltaTime;
}

