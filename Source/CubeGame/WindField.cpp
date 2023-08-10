// Fill out your copyright notice in the Description page of Project Settings.


#include "WindField.h"

#include "CubeGameCharacter.h"
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
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture2D");
	SceneCaptureComponent2D->SetupAttachment(RootComponent);
	SceneCaptureComponent2D->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCaptureComponent2D->CaptureSource = ESceneCaptureSource::SCS_DeviceDepth;
	SceneCaptureComponent2D->PostProcessBlendWeight = 0.0;
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
		if (CubeGameCharacter && CubeGameCharacter->GetCurrentRelaxRate() < 1.0f)
		{
			TArray<FName> BoneNames;
			SkeletalMesh->GetBoneNames(BoneNames);
			for (FName& BoneName: BoneNames)
			{
				if (FBodyInstance* BodyInstance = SkeletalMesh->GetBodyInstance(BoneName))
				{
				
					const float SurfaceArea = CalcSurfaceArea(BodyInstance);
					const FVector Force = CalcWindLoad(SurfaceArea);
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
			const FVector CaptureLocation = GravityCenter - UKismetMathLibrary::Normal(WindDirection.Vector()) * CaptureDistance;
			
			//TODO shadowmap
			UTextureRenderTarget2D* RTDepthMap = UKismetRenderingLibrary::CreateRenderTarget2D(this, 256, 256, ETextureRenderTargetFormat::RTF_RGBA16f);
			SceneCaptureComponent2D->ShowOnlyActors.Empty();
			SceneCaptureComponent2D->ShowOnlyActors.Add(Actor);
			SceneCaptureComponent2D->TextureTarget = RTDepthMap;
			SceneCaptureComponent2D->OrthoWidth = UKismetMathLibrary::Max(Bounds.GetSize().X*1.732, Bounds.GetSize().Y*1.732);
			SceneCaptureComponent2D->SetWorldLocation(CaptureLocation);
			SceneCaptureComponent2D->SetWorldRotation(WindDirection);
			SceneCaptureComponent2D->CaptureScene();

			//TODO performance optimization
			FRenderTarget* RenderTarget = RTDepthMap->GameThread_GetRenderTargetResource();
			TArray<FColor> Pixels;
			FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
			RenderTarget->ReadPixels(Pixels, ReadPixelFlags);
			const FIntPoint TextureSize = RenderTarget->GetSizeXY();
			
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
			float PixelX = SceneCaptureComponent2D->OrthoWidth / TextureSize.X * 0.01; 
			float PixelY = SceneCaptureComponent2D->OrthoWidth / TextureSize.Y * 0.01;
			float PixelArea = PixelX * PixelY;

			int PixelCount = 0;
			FVector TotalR = FVector(0, 0, 0);
			
			for (int i = 0; i < TextureSize.X; i++)
			{
				for (int j = 0; j < TextureSize.Y; j++)
				{
					if (Pixels[i * TextureSize.Y + j] != FColor::Black)
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
			
			FVector RightVector = SceneCaptureComponent2D->GetRightVector();
			FVector UpVector = SceneCaptureComponent2D->GetUpVector();
			FVector RVector = -TotalR.X * PixelX * UpVector + TotalR.Y * PixelY * RightVector;
			const float SurfaceArea = PixelCount * PixelArea;

			const FVector TotalForce = CalcWindLoad(SurfaceArea);
			const FVector TotalTorque = UKismetMathLibrary::Cross_VectorVector(RVector, TotalForce);
			CubeGameCharacter->GetMesh()->AddForce(TotalForce);
			CubeGameCharacter->GetMesh()->AddTorqueInRadians(TotalTorque);

			UKismetSystemLibrary::PrintString(this, TextureSize.ToString());
			UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_IntToString(PixelCount));
			UKismetSystemLibrary::PrintString(this, TotalR.ToString());
			UKismetSystemLibrary::PrintString(this, (GravityCenter+RVector).ToString());
			UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(SurfaceArea));
			UKismetSystemLibrary::PrintString(this, TotalForce.ToString());
			UKismetSystemLibrary::PrintString(this, TotalTorque.ToString());
		}
	}
	else if (UStaticMeshComponent* StaticMesh = Actor->GetComponentByClass<UStaticMeshComponent>())
	{
		if (FBodyInstance* BodyInstance = StaticMesh->GetBodyInstance())
		{
			const float SurfaceArea = CalcSurfaceArea(BodyInstance);
			const FVector Force = CalcWindLoad(SurfaceArea);
			BodyInstance->AddForce(Force);
		}
	}
}

FVector AWindField::CalcWindLoad(float WindSurfaceArea)
{
	const float rho = 1.204;
	return 0.5* rho * WindSurfaceArea* UKismetMathLibrary::Normal(UKismetMathLibrary::GetForwardVector(WindDirection)) * WindStrength * WindStrength;
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

float AWindField::CalcSurfaceArea(FBodyInstance* BodyInstance)
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
	for (auto& Normal: GetCubeNormals(UKismetMathLibrary::GetForwardVector(BodyInstance->GetUnrealWorldTransform().Rotator())))
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
	return TotalArea;
	
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

