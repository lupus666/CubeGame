// Copyright Epic Games, Inc. All Rights Reserved.

#include "CubeGameCharacter.h"

#include "CubeAnimInstance.h"
#include "GravityVolumeBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InputCoreTypes.h"
#include "WindField.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "CubeAbilityBlackHole.h"
#include "CubeAbilityDilationDefense.h"
#include "CubeAbilityGrab.h"
#include "CubeAbilityShoot.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


//////////////////////////////////////////////////////////////////////////
// ACubeGameCharacter

ACubeGameCharacter::ACubeGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(3, 3);
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 50.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 0.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	// GetCharacterMovement()->UpdatedComponent = nullptr;
	
	GetCameraBoom()->bEnableCameraLag = true;
	GetCameraBoom()->CameraLagSpeed = 15.0f;
	GetCameraBoom()->CameraLagMaxDistance = 20.0f;
	GetCameraBoom()->SetRelativeLocation(FVector(0, 0, CenterHeight));
	// RootComponent = GetMesh();
	PhysicalAnimationComponent = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicalAnimation"));

	SprintTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("SprintTimelineComponent"));
	// MovementPhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("MovementPhysicsConstraint"));
	// MovementPhysicsConstraint->SetupAttachment(RootComponent);
	// MovementPhysicsConstraint->SetRelativeLocation(FVector(0, 0, 14.5f));

	// MovementPhysicsConstraint->SetupAttachment(ConstraintActor);

	RollTorque = 10000000.0f;
	MaxTorque = 50000000.0f;
	JumpImpulse = 350000.0f;
	MaxPower = 10000000.0f;
	CurrentPower = MaxPower;
	bConstantPower =  true;
}

void ACubeGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	PhysicalAnimationComponent->SetStrengthMultiplyer(CurrentRelaxRate);
	UpdateMaterialCollection();
	if (!GetMesh()->IsSimulatingPhysics(BodyName))
	{
		// GetMesh()->SetWorldLocation(UKismetMathLibrary::VectorSpringInterp(GetMesh()->GetComponentLocation(),
		// GetCharacterMovement()->GetActorLocation() + FVector(0, 0, 3.0f), MovementSpring, 10, 1, DeltaSeconds));
		// GetMesh()->SetWorldLocation(GetCharacterMovement()->GetActorLocation());
		GetMesh()->AddWorldOffset((GetCapsuleComponent()->GetComponentLocation() - GetMesh()->GetComponentLocation()) * DeltaSeconds, false);
	}
	else
	{
		// if (bIsSphere)
		{
			const FVector DragDirection = UKismetMathLibrary::Normal(GetMesh()->GetPhysicsLinearVelocity(BodyName));
			GetMesh()->AddForce(-0.3*0.35*0.35*1.29*DragDirection*UKismetMathLibrary::VSizeSquared(GetMesh()->GetPhysicsLinearVelocity(BodyName)), BodyName, false);
			UpdateRootMovement(DeltaSeconds);
		}
		// else if (bIsInWindField)
		// {
		// 	FVector TargetLocation = GetMesh()->GetComponentLocation();
		// 	SetActorLocation(TargetLocation, false);
		// }
		// else
		// {
		// 	UpdateMovementConstraint(DeltaSeconds);
		// 	if (CurrentRelaxRate < 10.0f)
		// 	{
		// 		GetCharacterMovement()->MaxWalkSpeed = 120.0f;
		// 	}
		// 	else
		// 	{
		// 		GetCharacterMovement()->MaxWalkSpeed = 50.0f;
		// 	}
		// }
	}
}

void ACubeGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	CurrentRelaxRate = RelaxRate;
	// GetCapsuleComponent()->SetSimulatePhysics(true);

	// Set Timeline
	if (SprintCurve)
	{
		FOnTimelineFloat OnTimelineFloat;
		OnTimelineFloat.BindUFunction(this, TEXT("OnSprintTimelineTick"));
		SprintTimelineComponent->AddInterpFloat(SprintCurve, OnTimelineFloat);
	}

	// Init Animation
	AnimInstance = GetMesh()? Cast<UCubeAnimInstance>(GetMesh()->GetAnimInstance()) : nullptr;
	if (InitMontage && AnimInstance)
	{
		bPreventInput = true;
		PlayAnimMontage(InitMontage);
		FOnMontageEnded OnInitMontageEnded;
		OnInitMontageEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			if (Montage == InitMontage)
			{
				UKismetSystemLibrary::Delay(this, 0.5f, FLatentActionInfo(0, FMath::Rand(), TEXT("ToCube"), this));
			}
		});
		AnimInstance->Montage_SetEndDelegate(OnInitMontageEnded);
	}
	else
	{
		ToCube();
	}

	// Set Animation
	if (MMBAnim && AnimInstance)
	{
		if (UCubeAnimInstance* CubeAnimInstance = Cast<UCubeAnimInstance>(AnimInstance))
		{
			CubeAnimInstance->MMBAnim = MMBAnim;
		}
	}
	
	if (MountAnim && AnimInstance)
	{
		if (UCubeAnimInstance* CubeAnimInstance = Cast<UCubeAnimInstance>(AnimInstance))
		{
			CubeAnimInstance->MountAnim = MountAnim;
		}
	}

	//TODO Set Material


	//TODO Set UI Widget

	
	//TODO Set Niagara
	
}

FVector ACubeGameCharacter::GetMountLocation() const
{
	return MountLocation;
}

FName ACubeGameCharacter::GetMountBoneName() const
{
	return MountBoneName;
}

void ACubeGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACubeGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACubeGameCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ACubeGameCharacterBase::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACubeGameCharacterBase::LookUp);
	// PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	// PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("RotateForward", this, &ACubeGameCharacter::RotateForward);
	PlayerInputComponent->BindAxis("RotateRight", this, &ACubeGameCharacter::RotateRight);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACubeGameCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACubeGameCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACubeGameCharacter::EndSprint);
	PlayerInputComponent->BindAction("Tighten", IE_Pressed, this, &ACubeGameCharacter::BeginTighten);
	PlayerInputComponent->BindAction("Tighten", IE_Released, this, &ACubeGameCharacter::EndTighten);
	PlayerInputComponent->BindAction("Relax", IE_Pressed, this, &ACubeGameCharacter::BeginRelax);
	PlayerInputComponent->BindAction("Relax", IE_Released, this, &ACubeGameCharacter::EndRelax);
	PlayerInputComponent->BindAction("Dilation", IE_Pressed, this, &ACubeGameCharacter::TimeDilation);
	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ACubeGameCharacter::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ACubeGameCharacter::ZoomOut);
	PlayerInputComponent->BindAction("Mount", IE_Pressed, this, &ACubeGameCharacter::Aim);
	PlayerInputComponent->BindAction("Mount", IE_Released, this, &ACubeGameCharacter::Mount);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ACubeGameCharacter::Attack);
	PlayerInputComponent->BindAction("ToSphere", IE_Pressed, this, &ACubeGameCharacter::ToSphere);
	PlayerInputComponent->BindAction("ToCube", IE_Pressed, this, &ACubeGameCharacter::ToCube);
	PlayerInputComponent->BindAction("ToPlane", IE_Pressed, this, &ACubeGameCharacter::ToPlane);
	PlayerInputComponent->BindAction("ToFly", IE_Pressed, this, &ACubeGameCharacter::ToFly);

	PlayerInputComponent->BindAction("BlackHole", IE_Released, this, &ACubeGameCharacter::BlackHole);
	
	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &ACubeGameCharacter::BeginGrab);
	PlayerInputComponent->BindAction("Grab", IE_Released, this, &ACubeGameCharacter::EndGrab);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &ACubeGameCharacter::Throw);

	PlayerInputComponent->BindAction("Target", IE_Released, this, &ACubeGameCharacter::Target);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &ACubeGameCharacter::Shoot);
	
	PlayerInputComponent->BindAction("DilationDefense", IE_Pressed, this, &ACubeGameCharacter::BeginDilationDefense);
	PlayerInputComponent->BindAction("DilationDefense", IE_Released, this, &ACubeGameCharacter::EndDilationDefense);

	PlayerInputComponent->BindAction("RadialMagnetic", IE_Pressed, this, &ACubeGameCharacter::BeginRadialMagnetic);
	PlayerInputComponent->BindAction("RadialMagnetic", IE_Released, this, &ACubeGameCharacter::EndRadialMagnetic);
	PlayerInputComponent->BindAction("RadialImpulse", IE_Pressed, this, &ACubeGameCharacter::BeginRadialImpulse);
	PlayerInputComponent->BindAction("RadialImpulse", IE_Released, this, &ACubeGameCharacter::EndRadialImpulse);

}

FName ACubeGameCharacter::GetBodyName() const
{
	return BodyName;
}

void ACubeGameCharacter::SetPhysicalAnimation()
{
	PhysicalAnimationComponent->SetSkeletalMeshComponent(GetMesh());
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
	GetMesh()->SetAllBodiesPhysicsBlendWeight(1.0);
	GetMesh()->SetSimulatePhysics(true);
	// if (!bIsSphere)
	// {
	// 	SetUpMovementConstraint();
	// }

	bPreventInput = false;
}

void ACubeGameCharacter::SetUpMovementConstraint()
{
	// MovementPhysicsConstraint->SetConstrainedComponents(GetCapsuleComponent(), EName::None, GetMesh(), BodyName);
	MovementPhysicsConstraint->SetConstrainedComponents(GetMesh(), BodyName, GetCapsuleComponent(), EName::None);

	MovementPhysicsConstraint->SetRelativeLocation(FVector(0, 0, 14.5f));
	MovementPhysicsConstraint->SetConstraintReferencePosition(EConstraintFrame::Frame1, FVector(0, 0, 0));
	MovementPhysicsConstraint->SetConstraintReferencePosition(EConstraintFrame::Frame2, FVector(0, 0, 14.5));
	
	MovementPhysicsConstraint->SetLinearXLimit(LCM_Limited, XLimit);
	MovementPhysicsConstraint->SetLinearYLimit(LCM_Limited, YLimit);
	MovementPhysicsConstraint->SetLinearZLimit(LCM_Limited, ZLimit);

	// soft constraints
	MovementPhysicsConstraint->ConstraintInstance.SetSoftLinearLimitParams(true, Stiffness, Damping, Restitution, ContactDistance);
	ConstraintSpring.Reset();
	StiffnessSpring.Reset();
	DampingSpring.Reset();
}

void ACubeGameCharacter::UpdateMovementConstraint(float DeltaSeconds)
{
	const double Speed = UKismetMathLibrary::VSize(GetVelocity());
	// UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(Speed));

	// Spring for LinearLimit
	{
		float TargetLimit = UKismetMathLibrary::Sqrt(Speed) * UKismetMathLibrary::Loge(Speed + 1) / 4.0f;
		ConstraintSpring.bPrevTargetValid = false;
		XLimit = UKismetMathLibrary::FClamp(UKismetMathLibrary::FloatSpringInterp(XLimit, TargetLimit,
			ConstraintSpring, 20, 1, DeltaSeconds), 50.0f, 100.0f);
		YLimit = XLimit;
		ZLimit = XLimit;
		MovementPhysicsConstraint->SetLinearXLimit(LCM_Limited, XLimit);
		MovementPhysicsConstraint->SetLinearYLimit(LCM_Limited, YLimit);
		MovementPhysicsConstraint->SetLinearZLimit(LCM_Limited, ZLimit);
		// UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(XLimit));
	}

	// Spring for SoftLinearLimit
	{

		StiffnessSpring.bPrevTargetValid = false;
		DampingSpring.bPrevTargetValid = false;
	 	const float TargetStiffness = UKismetMathLibrary::Exp(-Speed)*100 + 20;
		const float TargetDamping = Speed * Speed / 1000;
		Stiffness = UKismetMathLibrary::FloatSpringInterp(Stiffness, TargetStiffness, StiffnessSpring, 10, 1, DeltaSeconds);
		Damping = UKismetMathLibrary::FloatSpringInterp(Damping, TargetDamping, DampingSpring, 10, 1, DeltaSeconds);
	 	MovementPhysicsConstraint->ConstraintInstance.SetSoftLinearLimitParams(true, Stiffness, Damping, Restitution, ContactDistance);
		// UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(Stiffness));
	}
	// Jump Setting
	if (bIsJumping && GetCharacterMovement()->IsFalling() && GetVelocity().Z >= 0.0f)
	{
		MovementPhysicsConstraint->SetLinearXLimit(LCM_Locked, XLimit);
		MovementPhysicsConstraint->SetLinearYLimit(LCM_Locked, YLimit);
		MovementPhysicsConstraint->SetLinearZLimit(LCM_Locked, ZLimit);
	}
}

void ACubeGameCharacter::CancelMovementConstraint()
{
	MovementPhysicsConstraint->SetLinearXLimit(LCM_Free, XLimit);
	MovementPhysicsConstraint->SetLinearXLimit(LCM_Free, YLimit);
	MovementPhysicsConstraint->SetLinearXLimit(LCM_Free, ZLimit);
}

void ACubeGameCharacter::MoveForward(float Value)
{
	if (!bPreventInput)
	{
		ForwardSpring.bPrevTargetValid = false;
		ForwardValue = UKismetMathLibrary::FloatSpringInterp(ForwardValue, Value, ForwardSpring, 20, 1, GetWorld()->GetDeltaSeconds());
		// if (bIsSphere)
		if (Controller && abs(ForwardValue - 0.0) > UE_SMALL_NUMBER)
		{
			FVector Direction = GetCameraBoom()->GetRightVector();

			if (GetMesh()->IsSimulatingPhysics())
			{
				const float Omega = UKismetMathLibrary::VSizeXY(GetMesh()->GetPhysicsAngularVelocityInRadians(BodyName));
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
							TargetTorque = CurrentPower / Omega;
						}
					}
					// TargetTorque *= ForwardValue;
					SphereForwardSpring.bPrevTargetValid = false;
					CurrentTorque = UKismetMathLibrary::FloatSpringInterp(CurrentTorque, TargetTorque, SphereForwardSpring, 10, 1, GetWorld()->GetDeltaSeconds());

					const FVector Torque = Direction * CurrentTorque * ForwardValue;
					GetMesh()->AddTorqueInRadians(Torque, BodyName, false);
					// Cube->AddTorqueInRadians(FVector(0, Value * CurrentTorque, 0), BodyName, false);
				}
			}
			else
			{
				// const FRotator Rotation = Controller->GetControlRotation();
				const FRotator Rotation = GetCameraBoom()->GetComponentRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);
				Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				// AddMovementInput(Direction, ForwardValue);
				GetCapsuleComponent()->AddWorldOffset(ForwardValue*Direction*GetCharacterMovement()->MaxWalkSpeed*GetWorld()->GetDeltaSeconds());
				// UKismetSystemLibrary::PrintString(this, Direction.ToString());
				// UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(ForwardValue));

			}
		}
		else
		{
			SphereForwardSpring.bPrevTargetValid = false;
			CurrentTorque = UKismetMathLibrary::FloatSpringInterp(CurrentTorque, 0, SphereForwardSpring, 10, 1, GetWorld()->GetDeltaSeconds());
		}

		// else
		// {
		// 	// const FRotator Rotation = Controller->GetControlRotation();
		// 	// const FRotator YawRotation(0, Rotation.Yaw, 0);
		// 	// FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// 	// GetCapsuleComponent()->AddForce(Direction * 100 * ForwardValue);
		// 	ACubeGameCharacterBase::MoveForward(ForwardValue);
		// }
	}
}

void ACubeGameCharacter::MoveRight(float Value)
{
	if (!bPreventInput)
	{
		RightSpring.bPrevTargetValid = false;
		RightValue = UKismetMathLibrary::FloatSpringInterp(RightValue, Value, RightSpring, 20, 1, GetWorld()->GetDeltaSeconds());
		// if (bIsSphere)
		if (Controller && abs(RightValue - 0.0) > UE_SMALL_NUMBER)
		{
			FVector Direction = GetCameraBoom()->GetForwardVector();


			if (GetMesh()->IsSimulatingPhysics())
			{
				const float Omega = UKismetMathLibrary::VSizeXY(GetMesh()->GetPhysicsAngularVelocityInRadians(BodyName));
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
							TargetTorque = CurrentPower / Omega;
						}
					}
					// TargetTorque *= RightValue;
					SphereForwardSpring.bPrevTargetValid = false;
					CurrentTorque = UKismetMathLibrary::FloatSpringInterp(CurrentTorque, TargetTorque, SphereForwardSpring, 10, 1, GetWorld()->GetDeltaSeconds());
			
					const FVector Torque = -Direction * CurrentTorque * RightValue;
		
					GetMesh()->AddTorqueInRadians(Torque, BodyName, false);
					// Cube->AddTorqueInRadians(FVector(0, Value * CurrentTorque, 0), BodyName, false);
				}
			}
			else
			{
				const FRotator Rotation = GetCameraBoom()->GetComponentRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);
				Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				GetCapsuleComponent()->AddWorldOffset(RightValue*Direction*GetCharacterMovement()->MaxWalkSpeed*GetWorld()->GetDeltaSeconds());
				// AddMovementInput(Direction, RightValue);
			}
		}
		else
		{
			SphereForwardSpring.bPrevTargetValid = false;
			CurrentTorque = UKismetMathLibrary::FloatSpringInterp(CurrentTorque, 0, SphereForwardSpring, 10, 1, GetWorld()->GetDeltaSeconds());
		}
		// else
		// {
		// 	// const FRotator Rotation = Controller->GetControlRotation();
		// 	// const FRotator YawRotation(0, Rotation.Yaw, 0);
		// 	// FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// 	// GetCapsuleComponent()->AddForce(Direction * 100 * RightValue);
		// 	ACubeGameCharacterBase::MoveRight(RightValue);
		//
		// }
	}
}

void ACubeGameCharacter::UpdateRootMovement(float DeltaSeconds)
{
	FVector TargetLocation = GetMesh()->GetBoneLocation(BodyName);
	FVector CurrentLocation = GetActorLocation();
	RootMovementSpring.bPrevTargetValid = false;
	CurrentLocation = UKismetMathLibrary::VectorSpringInterp(CurrentLocation, TargetLocation, RootMovementSpring, 20, 1, DeltaSeconds);
	SetActorLocation(CurrentLocation, false);
}

void ACubeGameCharacter::BeginSprint()
{
	if (CubeState == EShapeType::Sphere)
	{
		SprintTimelineComponent->Play();
	}
}

void ACubeGameCharacter::EndSprint()
{
	if (CubeState == EShapeType::Sphere)
	{
		SprintTimelineComponent->Reverse();
	}
}

void ACubeGameCharacter::OnSprintTimelineTick(float Value)
{
	CurrentPower = Value / SprintCurve->GetFloatValue(0.0) * MaxPower;
	GetCharacterMovement()->MaxWalkSpeed = Value;
}

// TODO Land BUG
void ACubeGameCharacter::Jump()
{
	if (!bPreventInput)
	{
		if (UCubeAnimInstance* CubeAnimInstance = Cast<UCubeAnimInstance>(AnimInstance))
		{
			// if(!bIsSphere)
			// {
			// 	if (CubeAnimInstance->JumpCount < 2)
			// 	{
			// 		CubeAnimInstance->JumpCount += 1;
			// 		CubeAnimInstance->bIsJumping = true;
			// 		bIsJumping = true;
			// 		if (CubeAnimInstance->JumpCount == 1)
			// 		{
			// 			GetWorldTimerManager().SetTimer(JumpTimerHandle, [this]()
			// 			{
			// 				LaunchCharacter(FVector(0.0f, 0.0f, 600.0f), false, false);
			// 				GetWorldTimerManager().ClearTimer(JumpTimerHandle);
			//
			// 			}, .2f, false);
			// 		}
			// 		else
			// 		{
			// 			LaunchCharacter(FVector(0.0f, 0.0f, 600.0f), false, false);
			// 		}
			// 	}
			// }
			// else if (!bIsJumping)
			if (!bIsJumping)
			{
				bIsJumping = true;
				GetMesh()->AddImpulse(FVector(0.f, 0.f, JumpImpulse));
			}
		}
		else
		{
			bIsJumping = true;
			Super::Jump();
		}

	}
}

//TODO Fix Landed BUG
void ACubeGameCharacter::Landed(const FHitResult& Hit)
{
	bIsJumping = false;
	if (UCubeAnimInstance* CubeAnimInstance = Cast<UCubeAnimInstance>(AnimInstance))
	{
		CubeAnimInstance->bIsJumping = false;
		CubeAnimInstance->JumpCount = 0;
	}
	else
	{
		Super::Landed(Hit);
	}
}

void ACubeGameCharacter::BeginTighten()
{
	if (!bPreventInput && GetMesh()->IsSimulatingPhysics())
	{
		ACubeMountCharacter::BeginTighten();
	}
}

void ACubeGameCharacter::BeginRelax()
{
	if (!bPreventInput)
	{
		ACubeMountCharacter::BeginRelax();
	}
}

void ACubeGameCharacter::Aim()
{
	// GetCameraBoom()->SetRelativeLocation(FVector(0, 0, 50.0f));
	//TODO HUD
	GetCameraBoom()->bDoCollisionTest = true;
	PhysicalAnimationComponent->SetStrengthMultiplyer(RelaxRate);
	// TODO Character Rotation with Camera
}

void ACubeGameCharacter::Mount()
{
	GetCameraBoom()->bDoCollisionTest = false;
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	FVector Start = CameraManager->K2_GetActorLocation();
	FVector End = CameraManager->GetActorForwardVector()*MountDistance + Start;
	FHitResult OutHit;
	FCollisionQueryParams TraceParams(FName("MountTrace"), false, GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, TraceParams))
	{
		if (ACubeMountCharacter* MountCharacter = Cast<ACubeMountCharacter>(OutHit.GetActor()))
		{
			GetCameraBoom()->SetRelativeLocation(FVector(0, 0, 0));
			MountLocation = OutHit.Location;
			MountBoneName = OutHit.BoneName;
			MountCharacter->OnMount(this);
			bIsMounted = true;
		}
	}
	
	// GetCameraBoom()->SetRelativeLocation(FVector(0, 0, 10.0f));
	GetCameraBoom()->bDoCollisionTest = true;

}

void ACubeGameCharacter::Attack()
{
	if (AnimInstance && !bIsAttack && !bPreventInput && AttackMontage)
	{
		bIsAttack = true;
		PlayAnimMontage(AttackMontage);
		FOnMontageEnded OnAttackMontageEnded;
		OnAttackMontageEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			if (Montage == AttackMontage)
			{
				bIsAttack = false;
			}
		});
		AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded);
	}
}

void ACubeGameCharacter::ToSphere()
{
	if (SphereSequence && SpherePhysicsAsset && CurrentRelaxRate > 99.0f)
	{
		GetMesh()->GetBodyInstance(BodyName)->SetInstanceSimulatePhysics(false, false,true);
		GetMesh()->SetPhysicsAsset(SpherePhysicsAsset, false);
		GetMesh()->bUpdateJointsFromAnimation = true;
		// GetMesh()->bUpdateMeshWhenKinematic = true;
		// GetMesh()->bIncludeComponentLocationIntoBounds = true;
		if (AnimInstance && !AnimInstance->bIsMorphing)
		{
			AnimInstance->ChangeShape(EShapeType::Sphere);
			SetPhysicalAnimation();
			// UKismetSystemLibrary::Delay(this, 0.7, FLatentActionInfo(0, FMath::Rand(), TEXT("SetPhysicalAnimation"), this));
		}
		else
		{
			GetMesh()->PlayAnimation(SphereSequence, false);
			SetPhysicalAnimation();
		}
		CubeState = EShapeType::Sphere;
	}
}

void ACubeGameCharacter::ToCube()
{
	if (CubeSequence && CubePhysicsAsset && CurrentRelaxRate > 99.0f)
	{
		GetMesh()->GetBodyInstance(BodyName)->SetInstanceSimulatePhysics(false, false,true);
		GetMesh()->SetPhysicsAsset(CubePhysicsAsset, false);
		GetMesh()->bUpdateJointsFromAnimation = true;
		// GetMesh()->bUpdateMeshWhenKinematic = true;
		// GetMesh()->bIncludeComponentLocationIntoBounds = true;
		{
			SetActorLocation(GetMesh()->GetComponentLocation(), false);
		}
		if (AnimInstance && !AnimInstance->bIsMorphing)
		{
			AnimInstance->ChangeShape(EShapeType::Cube);
			// UKismetSystemLibrary::Delay(this, 0.7, FLatentActionInfo(0, FMath::Rand(), TEXT("SetPhysicalAnimation"), this));
			SetPhysicalAnimation();
		}
		else
		{
			GetMesh()->PlayAnimation(CubeSequence, false);
			SetPhysicalAnimation();
		}
		CubeState = EShapeType::Cube;
	}
}

void ACubeGameCharacter::ToPlane()
{
	if (PlaneSequence && PlanePhysicsAsset && CurrentRelaxRate > 99.0f)
	{
		// GetMesh()->SetSimulatePhysics(false);
		GetMesh()->GetBodyInstance(BodyName)->SetInstanceSimulatePhysics(false, false,true);
		GetMesh()->SetPhysicsAsset(PlanePhysicsAsset, true);
		GetMesh()->bUpdateJointsFromAnimation = true;
		// GetMesh()->bUpdateMeshWhenKinematic = true;
		// GetMesh()->bIncludeComponentLocationIntoBounds = true;
		
		{
			SetActorLocation(GetMesh()->GetComponentLocation(), false);
		}
		if (AnimInstance && !AnimInstance->bIsMorphing)
		{
			AnimInstance->ChangeShape(EShapeType::Plane);
			// UKismetSystemLibrary::Delay(this, 0.7, FLatentActionInfo(0, FMath::Rand(), TEXT("SetPhysicalAnimation"), this));
			SetPhysicalAnimation();
		}
		else
		{
			GetMesh()->PlayAnimation(PlaneSequence, false);
			SetPhysicalAnimation();
		}
		CubeState = EShapeType::Plane;
	}
}

void ACubeGameCharacter::ToFly()
{
	if (FlySequence && FlyPhysicsAsset && CurrentRelaxRate > 99.0f)
	{
		GetMesh()->SetSimulatePhysics(false);
		// GetMesh()->GetBodyInstance(BodyName)->SetInstanceSimulatePhysics(false, false,true);
		GetMesh()->SetPhysicsAsset(FlyPhysicsAsset, true);
		GetMesh()->bUpdateJointsFromAnimation = true;
		// GetMesh()->bUpdateMeshWhenKinematic = true;
		// GetMesh()->bIncludeComponentLocationIntoBounds = true;
		{
			SetActorLocation(GetMesh()->GetComponentLocation(), false);
		}
		if (AnimInstance && !AnimInstance->bIsMorphing)
		{
			AnimInstance->ChangeShape(EShapeType::Fly);
			// UKismetSystemLibrary::Delay(this, 0.7, FLatentActionInfo(0, FMath::Rand(), TEXT("SetPhysicalAnimation"), this));
			SetPhysicalAnimation();
		}
		else
		{
			GetMesh()->PlayAnimation(FlySequence, false);
			SetPhysicalAnimation();
		}
		CubeState = EShapeType::Fly;
	}
}

void ACubeGameCharacter::RotateForward(float Value)
{
	if (CubeState == EShapeType::Cube && Value != 0.0f && CurrentRelaxRate >= 99.0f)
	{
		TArray<AActor*> OverlappingActors;
		GetCapsuleComponent()->GetOverlappingActors(OverlappingActors);
		for (auto& Actor: OverlappingActors)
		{
			if (AGravityVolumeBase* GravityVolume= Cast<AGravityVolumeBase>(Actor))
			{
				if (GravityVolume->bCanRotate)
				{
					// Rotate Gravity Based On Camera Axis
					{
						FVector CameraRotationAxis = GetCameraBoom()->GetRightVector().GetSafeNormal() * (Value > 0.0f ? 1: -1);
						FVector CubeRotationAxis;
						for (auto& Normal: AWindField::GetCubeNormals(GetMesh()->GetForwardVector()))
						{
							float AngleDegree = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraRotationAxis, Normal)));
							if (AngleDegree >= 0.0f && AngleDegree < 45.0f)
							{
								CubeRotationAxis = Normal;
								break;
							}
						}
						FVector GravityDirection = GravityVolume->GravityDirection;
						GravityDirection = UKismetMathLibrary::RotateAngleAxis(GravityDirection, DeltaRotateAngle, CubeRotationAxis.GetSafeNormal()).GetSafeNormal();
						GravityVolume->GravityDirection = GravityDirection;
						// UKismetSystemLibrary::PrintString(this, CubeRotationAxis.ToString());
						// UKismetSystemLibrary::PrintString(this, GravityDirection.ToString());
					}
					
					// Rotate Gravity Based On GravityVolume
					{
						// FVector RotationAxis = Cast<UShapeComponent>(GravityVolume->GetComponentByClass(UShapeComponent::StaticClass()))->GetRightVector().GetSafeNormal() * (Value > 0.0f ? 1: -1);
						// FVector GravityDirection = UKismetMathLibrary::RotateAngleAxis(GravityVolume->GravityDirection, 90.0f, RotationAxis).GetSafeNormal();
						// UKismetSystemLibrary::PrintString(this, RotationAxis.ToString());
						// UKismetSystemLibrary::PrintString(this, GravityDirection.ToString());
						//
						// GravityVolume->GravityDirection = FVector(-1, 0, 0);
					}
				}
			}
		}
	}
	
}

void ACubeGameCharacter::RotateRight(float Value)
{
	if (CubeState == EShapeType::Cube && Value != 0.0f && CurrentRelaxRate >= 99.0f)
	{
		TArray<AActor*> OverlappingActors;
		GetCapsuleComponent()->GetOverlappingActors(OverlappingActors);
		for (auto& Actor: OverlappingActors)
		{
			if (AGravityVolumeBase* GravityVolume= Cast<AGravityVolumeBase>(Actor))
			{
				if (GravityVolume->bCanRotate)
				{
					FVector CameraRotationAxis = GetCameraBoom()->GetForwardVector().GetSafeNormal() * (Value > 0.0f ? 1: -1);
					FVector CubeRotationAxis = CameraRotationAxis;
					// for (auto& Normal: AWindField::GetCubeNormals(GetMesh()->GetForwardVector()))
					// {
					// 	float AngleDegree = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraRotationAxis, Normal)));
					// 	if (AngleDegree < 45.0f)
					// 	{
					// 		CubeRotationAxis = Normal;
					// 		break;
					// 	}
					// }
					FVector GravityDirection = GravityVolume->GravityDirection;
					GravityDirection = UKismetMathLibrary::RotateAngleAxis(GravityDirection, DeltaRotateAngle, CubeRotationAxis);
					GravityVolume->GravityDirection = GravityDirection;
				}
			}
		}
	}
}

void ACubeGameCharacter::UpdateMaterialCollection()
{
	if (MaterialParameterCollection)
	{
		FVector PlayerToCamera = GetFollowCamera()->GetComponentLocation() - GetMesh()->GetComponentLocation();
		FVector PlayerPosition = GetMesh()->GetComponentLocation();
		const FBoxSphereBounds Bounds = GetMesh()->Bounds;
		UKismetMaterialLibrary::SetVectorParameterValue(this, MaterialParameterCollection, FName("PlayerToCamera"), FLinearColor(PlayerToCamera.X, PlayerToCamera.Y, PlayerToCamera.Z));
		UKismetMaterialLibrary::SetVectorParameterValue(this, MaterialParameterCollection, FName("PlayerPosition"), FLinearColor(PlayerPosition.X, PlayerPosition.Y, PlayerPosition.Z));
	}
}

void ACubeGameCharacter::BlackHole()
{
	if (IsValid(AbilityBlackHole))
	{
		AbilityBlackHole->Destroy();
		AbilityBlackHole = nullptr;
	}
	else
	{
		AbilityBlackHole = Cast<ACubeAbilityBlackHole>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,
			ACubeAbilityBlackHole::StaticClass(), FTransform(GetActorLocation())));
		AbilityBlackHole->Initialize(this);
		UGameplayStatics::FinishSpawningActor(AbilityBlackHole, FTransform(GetActorLocation()));
	}
}

void ACubeGameCharacter::BeginDilationDefense()
{
	AbilityDilationDefense = Cast<ACubeAbilityDilationDefense>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,
		ACubeAbilityDilationDefense::StaticClass(), FTransform(GetActorLocation())));
	if (AbilityDilationDefense)
	{
		AbilityDilationDefense->Initialize(this);
		UGameplayStatics::FinishSpawningActor(AbilityDilationDefense, FTransform(GetActorLocation()));
	}
	
}

void ACubeGameCharacter::EndDilationDefense()
{
	if (IsValid(AbilityDilationDefense))
	{
		AbilityDilationDefense->Destroy();
		AbilityDilationDefense = nullptr;
	}
}

void ACubeGameCharacter::BeginGrab()
{
	if (CubeState == EShapeType::Cube)
	{
		AbilityGrab = Cast<ACubeAbilityGrab>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,
			ACubeAbilityGrab::StaticClass(), FTransform(GetActorLocation())));
		if (AbilityGrab)
		{
			AbilityGrab->Initialize(this);
			UGameplayStatics::FinishSpawningActor(AbilityGrab, FTransform(GetActorLocation()));
		}
	}

}

void ACubeGameCharacter::EndGrab()
{
	if (CubeState == EShapeType::Cube)
	{
		if (IsValid(AbilityGrab) && IsValid(AbilityGrab->GetGrabTarget()))
		{
			AbilityGrab->ReleaseGrabTarget();
			AbilityGrab->Destroy();
			AbilityGrab = nullptr;
		}
	}
	
}

void ACubeGameCharacter::Throw()
{
	if (CubeState == EShapeType::Cube)
	{
		if (IsValid(AbilityGrab) && IsValid(AbilityGrab->GetGrabTarget()))
		{
			AbilityGrab->ThrowGrabTarget();
			AbilityGrab->Destroy();
			AbilityGrab = nullptr;
		}
	}
}

void ACubeGameCharacter::Target()
{
	if (CubeState == EShapeType::Sphere)
	{
		if (IsValid(AbilityShoot))
		{
			AbilityShoot->FindGrabTarget();
		}
		else
		{
			AbilityShoot = Cast<ACubeAbilityShoot>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,
			ACubeAbilityShoot::StaticClass(), FTransform(GetActorLocation())));
			if (AbilityShoot)
			{
				AbilityShoot->Initialize(this);
				UGameplayStatics::FinishSpawningActor(AbilityShoot, FTransform(GetActorLocation()));
			}
			AbilityShoot->FindGrabTarget();
		}
	}
}

void ACubeGameCharacter::Shoot()
{
	if (CubeState == EShapeType::Sphere)
	{
		if (IsValid(AbilityShoot))
		{
			AbilityShoot->ShootGrabTarget();
		}
	}
}

void ACubeGameCharacter::BeginRadialImpulse()
{
}

void ACubeGameCharacter::EndRadialImpulse()
{
}

void ACubeGameCharacter::EndRadialMagnetic()
{
}

void ACubeGameCharacter::BeginRadialMagnetic()
{
}







