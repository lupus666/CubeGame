// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberCube.h"

#include "CubeAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

// Sets default values
ACyberCube::ACyberCube()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CubeMesh(TEXT("/Game/CyberCubes_Fractal/Mesh/SKM_CyberCubes_Fractal.SKM_CyberCubes_Fractal"));
	Cube = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CyberCube"));
	Cube->SetSkeletalMesh(CubeMesh.Object);
	Cube->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Cube->SetAngularDamping(0.1f);
	Cube->SetLinearDamping(0.1f);

	// Cube->BodyInstance.MassScale = 3.5f;
	// Cube->BodyInstance.MaxAngularVelocity = 800.0f;
	// Cube->SetNotifyRigidBodyCollision(true);
	RootComponent = Cube;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bDoCollisionTest = false;
	// SpringArm->SetUsingAbsoluteRotation(true); // Rotation of the ball should not affect rotation of boom
	// SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 3.f;
	SpringArm->SetRelativeLocation(FVector(0, 0, 17.5));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // We don't want the controller rotating the camera
	
	RollTorque = 10000000.0f;
	MaxTorque = 50000000.0f;
	JumpImpulse = 350000.0f;
	MaxPower = 10000000.0f;
	RelaxSpeed = 2.0f;
	bCanJump = true; //
	bConstantPower =  true;

	TightenTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TightenTimelineComponent"));
	DilationTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DilationTimelineComponent"));
	PhysicalAnimationComponent = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicalAnimation"));

}

// Called when the game starts or when spawned
void ACyberCube::BeginPlay()
{
	Super::BeginPlay();

	CurrentRelaxRate = RelaxRate;
	
	if (TightenCurve)
	{
		FOnTimelineFloat OnTimelineFloat;
		OnTimelineFloat.BindUFunction(this, TEXT("OnTightenTimelineTick"));
		TightenTimelineComponent->AddInterpFloat(TightenCurve, OnTimelineFloat);
	}
	
	if (DilationCurve)
	{
		FOnTimelineFloat OnTimelineFloat;
		OnTimelineFloat.BindUFunction(this, TEXT("OnDilationTimelineTick"));
		DilationTimelineComponent->AddInterpFloat(DilationCurve, OnTimelineFloat);
	}

	if (UAnimInstance* Anim = Cube->GetAnimInstance())
	{
		AnimInstance = Cast<UCubeAnimInstance>(Anim);
	}
	
	ToCube();
}

void ACyberCube::SetPhysicalAnimation()
{
	PhysicalAnimationComponent->SetSkeletalMeshComponent(Cube);
	FPhysicalAnimationData PhysicalAnimationData;
	PhysicalAnimationData.bIsLocalSimulation = false;
	PhysicalAnimationData.OrientationStrength = 1000.f;
	PhysicalAnimationData.AngularVelocityStrength = 15.f;
	PhysicalAnimationData.PositionStrength = 1000.f;
	PhysicalAnimationData.VelocityStrength = 30.f;
	PhysicalAnimationData.MaxAngularForce = 0.0f;
	PhysicalAnimationData.MaxLinearForce = 0.0f;
	
	PhysicalAnimationComponent->ApplyPhysicalAnimationSettingsBelow(BodyName, PhysicalAnimationData, false);
	Cube->SetAllBodiesPhysicsBlendWeight(1.0);
	// Cube->SetAllBodiesBelowSimulatePhysics(BodyName, true, false);
	Cube->SetSimulatePhysics(true);
}

void ACyberCube::MoveForward(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// physical movement
		if (Cube->IsSimulatingPhysics())
		{
			const float Omega = UKismetMathLibrary::VSizeXY(Cube->GetPhysicsAngularVelocityInRadians(BodyName));
			float TargetTorque = RollTorque;
			if (Omega <= MaxAngularVelocity)
			{
				if (Omega >= 1.0f)
				{
					if (TorqueCurve) {
						TargetTorque = TorqueCurve->GetFloatValue(Omega);
					}
					else if (bConstantPower)
					{
						TargetTorque = MaxPower / Omega;
					}
				}
				CurrentTorque = UKismetMathLibrary::FloatSpringInterp(CurrentTorque, TargetTorque, PhysicalForwardSprintState, 20, 1, GetWorld()->GetDeltaSeconds());
				
				const FVector Torque = Direction * CurrentTorque * Value;
			
				Cube->AddTorqueInRadians(Torque, BodyName, false);
				// Cube->AddTorqueInRadians(FVector(0, Value * CurrentTorque, 0), BodyName, false);
			}
		}
		else
		{
			// pinhole movement
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			// if (CurrentSpeed <= UE_SMALL_NUMBER)
			// {
			// 	CurrentSpeed = UKismetMathLibrary::VSizeXY(Cube->GetPhysicsLinearVelocity());
			// }
			CurrentSpeed = UKismetMathLibrary::FInterpTo(CurrentSpeed, RelaxSpeed, GetWorld()->GetDeltaSeconds(), 0.1);
			AddActorWorldOffset(Direction*Value*CurrentSpeed, false);
			//TODO Add rotation
		}

	}
}

void ACyberCube::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// physical movement
		if (Cube->IsSimulatingPhysics())
		{
			const float Omega = UKismetMathLibrary::VSizeXY(Cube->GetPhysicsAngularVelocityInRadians(BodyName));
			float TargetTorque = RollTorque;
			if (Omega <= MaxAngularVelocity)
			{
				if (TorqueCurve) {
					TargetTorque = TorqueCurve->GetFloatValue(Omega);
				}
				else if (bConstantPower)
				{
					TargetTorque = MaxPower / Omega;
				}
				CurrentTorque = UKismetMathLibrary::FloatSpringInterp(CurrentTorque, TargetTorque, PhysicalForwardSprintState, 20, 1, GetWorld()->GetDeltaSeconds());
				
				const FVector Torque = -Direction * CurrentTorque * Value;
			
				Cube->AddTorqueInRadians(Torque, BodyName, false);
				// Cube->AddTorqueInRadians(FVector(-Value * CurrentTorque, 0, 0), BodyName, false);
			}
		}
		else
		{
			// pinhole movement
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// if (CurrentSpeed <= UE_SMALL_NUMBER)
			// {
			// 	CurrentSpeed = UKismetMathLibrary::VSizeXY(Cube->GetPhysicsLinearVelocity());
			// }
			CurrentSpeed = UKismetMathLibrary::FInterpTo(CurrentSpeed, RelaxSpeed, GetWorld()->GetDeltaSeconds(), 0.1);
			AddActorWorldOffset(Direction*Value*CurrentSpeed, false);
			//TODO Add rotation
		}
	}
}

void ACyberCube::ZoomIn()
{
	SpringArm->TargetArmLength = UKismetMathLibrary::Clamp(SpringArm->TargetArmLength - 20.0f, 90.0f, 1000.0f);
	// SpringArm->SetRelativeLocation(FVector(0, 0, 10));
}

void ACyberCube::ZoomOut()
{
	SpringArm->TargetArmLength = UKismetMathLibrary::Clamp(SpringArm->TargetArmLength + 20.0f, 90.0f, 1000.0f);
	// SpringArm->SetRelativeLocation(FVector(0, 0, 10));
}

void ACyberCube::TimeDilation()
{
	if (bTimeDilation)
	{
		DilationTimelineComponent->Reverse();
		bTimeDilation = false;
	}
	else
	{
		DilationTimelineComponent->Play();
		bTimeDilation = true;
	}
}

void ACyberCube::OnDilationTimelineTick(float Value)
{
	UGameplayStatics::SetGlobalTimeDilation(this, Value);
}

void ACyberCube::BeginTighten()
{
	TightenCount += 1;
	if (TightenCount >= 2)
	{
		CurrentRelaxRate = RelaxRate;
		//TODO Debug
		if (UKismetMathLibrary::VSizeXY(GetVelocity()) > 1.0)
		{
				
		}
		else
		{
			// if (TightenMontage)
			// {
			// 	PlayAnimMontage(TightenMontage);
			// 	bPreventInput = true;
			// 	FOnMontageEnded TightenMontageEnded;
			// 	TightenMontageEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
			// 	{
			// 		bPreventInput = false;
			// 	});
			// 	AnimInstance->Montage_SetEndDelegate(TightenMontageEnded);
			// }
		}
		TightenTimelineComponent->SetNewTime((CurrentRelaxRate > 1.0f) ? TightenTime : 0.0f);
	}
	else
	{
		TightenTimelineComponent->Play();
	}
	// Double click in 0.5s
	GetWorldTimerManager().SetTimer(TightenTimerHandle, [this]()
	{
		TightenCount = 0;
		GetWorldTimerManager().ClearTimer(TightenTimerHandle);
	}, 0.5f, false);
}

void ACyberCube::EndTighten()
{
	TightenTimelineComponent->Stop();
}

void ACyberCube::BeginRelax()
{
	RelaxCount += 1;
	if (RelaxCount >= 2)
	{
		CurrentRelaxRate = 0.0001f;
		TightenTimelineComponent->SetNewTime((CurrentRelaxRate > 1.0f) ? TightenTime: 0.0f);
	}
	else
	{
		TightenTimelineComponent->Reverse();
	}
	// Double click in 0.5s
	GetWorldTimerManager().SetTimer(RelaxTimerHandle, [this]()
	{
		RelaxCount = 0;
		GetWorldTimerManager().ClearTimer(RelaxTimerHandle);
	}, 0.5f, false);
}

void ACyberCube::EndRelax()
{
	TightenTimelineComponent->Stop();
}

void ACyberCube::OnTightenTimelineTick(float Value)
{
	CurrentRelaxRate = Value;

	PhysicalAnimationComponent->SetStrengthMultiplyer(CurrentRelaxRate);

	// Relax for faster movement
	// if (CurrentRelaxRate < RelaxThreshold)
	// {
	// 	// Cube->GetBodyInstance(BodyName)->SetInstanceSimulatePhysics(false);
	// 	Cube->SetSimulatePhysics(false);
	// 	Cube->SetAllBodiesBelowSimulatePhysics(BodyName, true, false);
	// }
	// else
	// {
	// 	// Cube->GetBodyInstance(BodyName)->SetInstanceSimulatePhysics(true);
	// 	Cube->SetSimulatePhysics(true);
	// }
}

void ACyberCube::ToSphere()
{
	if (SphereSequence != nullptr && (SpherePhysicsAsset != nullptr) && CurrentRelaxRate == 100.0f)
	{
		// Cube->SetSimulatePhysics(false);
		Cube->SetPhysicsAsset(SpherePhysicsAsset, false);
		Cube->bUpdateJointsFromAnimation = true;
		if (AnimInstance && !AnimInstance->bIsMorphing)
		{
			AnimInstance->ChangeShape(EShapeType::Sphere);
			UKismetSystemLibrary::Delay(this, 0.7, FLatentActionInfo(0, FMath::Rand(), TEXT("SetPhysicalAnimation"), this));
		}
		else
		{
			Cube->SetAnimation(SphereSequence);
			SetPhysicalAnimation();
		}
		SpringArm->SetRelativeLocation(FVector(0, 0, 17.5));
	}
}

void ACyberCube::ToCube()
{
	if (CubePhysicsAsset != nullptr && CubeSequence != nullptr && CurrentRelaxRate == 100.0f)
	{
		// Cube->SetSimulatePhysics(false);
		Cube->SetPhysicsAsset(CubePhysicsAsset, true);
		Cube->bUpdateJointsFromAnimation = true;
		if (AnimInstance && !AnimInstance->bIsMorphing)
		{
			AnimInstance->ChangeShape(EShapeType::Cube);
			UKismetSystemLibrary::Delay(this, 0.5, FLatentActionInfo(0, FMath::Rand(), TEXT("SetPhysicalAnimation"), this));
		}
		else
		{
			Cube->SetAnimation(CubeSequence);
			SetPhysicalAnimation();
		}
		SpringArm->SetRelativeLocation(FVector(0, 0, 17.5));
	}
}

// Called every frame
void ACyberCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// atmospheric drag
	if (Cube->IsSimulatingPhysics())
	{
		Cube->AddForce(-0.3*0.17*0.17*1.29*Cube->GetPhysicsLinearVelocity(BodyName) * Cube->GetPhysicsLinearVelocity(BodyName), BodyName, false);
	}
	// UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(UKismetMathLibrary::VSizeXY(Cube->GetPhysicsAngularVelocityInRadians(BodyName))));
	// UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(CurrentTorque));
}

// Called to bind functionality to input
void ACyberCube::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACyberCube::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACyberCube::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ACyberCube::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ACyberCube::ZoomOut);
	PlayerInputComponent->BindAction("Dilation", IE_Pressed, this, &ACyberCube::TimeDilation);

	PlayerInputComponent->BindAction("Tighten", IE_Pressed, this, &ACyberCube::BeginTighten);
	PlayerInputComponent->BindAction("Tighten", IE_Released, this, &ACyberCube::EndTighten);
	PlayerInputComponent->BindAction("Relax", IE_Pressed, this, &ACyberCube::BeginRelax);
	PlayerInputComponent->BindAction("Relax", IE_Released, this, &ACyberCube::EndRelax);

	PlayerInputComponent->BindAction("ToCube", IE_Pressed, this, &ACyberCube::ToCube);
	PlayerInputComponent->BindAction("ToSphere", IE_Pressed, this, &ACyberCube::ToSphere);
}

