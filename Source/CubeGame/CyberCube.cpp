// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberCube.h"

#include "LocalizationConfigurationScript.h"
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
	// Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CyberCube"));
	// Cube->SetStaticMesh()
	Cube->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Cube->SetSimulatePhysics(true);
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
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	RollTorque = 50000000.0f;
	MaxTorque = 20000000.0;
	JumpImpulse = 350000.0f;
	bCanJump = true; //

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

	if (PhysicsAsset != nullptr)
	{
		Cube->SetPhysicsAsset(PhysicsAsset, false);
		Cube->bUpdateJointsFromAnimation = true;
		// GetMesh()->bUpdateMeshWhenKinematic = true;
		// GetMesh()->bIncludeComponentLocationIntoBounds = true;
	}
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
		// GetMesh()->SetAllBodiesBelowSimulatePhysics(BodyName, true, false);
		// Cube->SetSimulatePhysics(true);
		// GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(BodyName, 1.0, false, false);
}

void ACyberCube::MoveForward(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f))
	{
		if (UKismetMathLibrary::VSizeXY(Cube->GetPhysicsAngularVelocityInRadians(BodyName)) <= MaxAngularVelocity && UKismetMathLibrary::VSizeXY(Cube->GetPhysicsLinearVelocity(BodyName)) <= MaxLinearVelocity)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			const FVector Torque = Direction * RollTorque * Value;
			CurrentTorque = UKismetMathLibrary::VectorSpringInterp(CurrentTorque, Torque, ForwardSprintState, 20, 1, GetWorld()->GetDeltaSeconds());
			Cube->AddTorqueInRadians(CurrentTorque, BodyName);
		}
	}
}

void ACyberCube::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f))
	{
		if (UKismetMathLibrary::VSizeXY(Cube->GetPhysicsAngularVelocityInRadians(BodyName)) <= MaxAngularVelocity && UKismetMathLibrary::VSizeXY(Cube->GetPhysicsLinearVelocity(BodyName)) <= MaxLinearVelocity)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
	
			// get right vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector Torque = -Direction * RollTorque * Value;
			CurrentTorque = UKismetMathLibrary::VectorSpringInterp(CurrentTorque, Torque, ForwardSprintState, 20, 1, GetWorld()->GetDeltaSeconds());
			Cube->AddTorqueInRadians(CurrentTorque, BodyName);
		}
	}
}

void ACyberCube::ZoomIn()
{
	SpringArm->TargetArmLength = UKismetMathLibrary::Clamp(SpringArm->TargetArmLength - 20.0f, 90.0f, 1000.0f);
	SpringArm->SetRelativeLocation(FVector(0, 0, 10));
}

void ACyberCube::ZoomOut()
{
	SpringArm->TargetArmLength = UKismetMathLibrary::Clamp(SpringArm->TargetArmLength + 20.0f, 90.0f, 1000.0f);
	SpringArm->SetRelativeLocation(FVector(0, 0, 10));
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
}

// Called every frame
void ACyberCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	PhysicalAnimationComponent->SetStrengthMultiplyer(CurrentRelaxRate);
	
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
	

}

