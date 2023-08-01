// Copyright Epic Games, Inc. All Rights Reserved.

#include "CubeGameCharacter.h"

#include "CubeAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InputCoreTypes.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
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
	GetCharacterMovement()->MaxWalkSpeed = 120.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 0.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	GetCameraBoom()->bEnableCameraLag = true;
	GetCameraBoom()->CameraLagSpeed = 15.0f;
	GetCameraBoom()->CameraLagMaxDistance = 20.0f;
	GetCameraBoom()->AddLocalOffset(FVector(0, 0, 11.0f));
	// RootComponent = GetMesh();
	PhysicalAnimationComponent = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicalAnimation"));

	SprintTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("SprintTimelineComponent"));

	MovementPhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("MovementPhysicsConstraint"));
	MovementPhysicsConstraint->SetupAttachment(RootComponent);
}

void ACubeGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PhysicalAnimationComponent->SetStrengthMultiplyer(CurrentRelaxRate);

	if (!GetMesh()->IsSimulatingPhysics(BodyName))
	{
		// GetMesh()->SetWorldLocation(UKismetMathLibrary::VectorSpringInterp(GetMesh()->GetComponentLocation(),
		// 	GetCharacterMovement()->GetActorLocation() + FVector(0, 0, 3.0f), MovementSpring, 10, 1, DeltaSeconds));
		// GetMesh()->SetWorldLocation(GetCharacterMovement()->GetActorLocation());
		GetMesh()->AddWorldOffset(GetCharacterMovement()->Velocity*DeltaSeconds);
	}
	else
	{
		UpdateMovementConstraint(DeltaSeconds);
	}
}

void ACubeGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Set Timeline
	if (SprintCurve)
	{
		FOnTimelineFloat OnTimelineFloat;
		OnTimelineFloat.BindUFunction(this, TEXT("OnSprintTimelineTick"));
		SprintTimelineComponent->AddInterpFloat(SprintCurve, OnTimelineFloat);
	}

	// Init Animation
	AnimInstance = (GetMesh())? GetMesh()->GetAnimInstance() : nullptr;
	if (InitMontage && AnimInstance)
	{
		bPreventInput = true;
		PlayAnimMontage(InitMontage);
		FOnMontageEnded OnInitMontageEnded;
		OnInitMontageEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			if (Montage == InitMontage)
			{
				UKismetSystemLibrary::Delay(this, 0.5f, FLatentActionInfo(0, FMath::Rand(), TEXT("OnPhysicsInit"), this));
			}
		});
		AnimInstance->Montage_SetEndDelegate(OnInitMontageEnded);
	}
	else
	{
		OnPhysicsInit();
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
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	
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
}

FName ACubeGameCharacter::GetBodyName() const
{
	return BodyName;
}

void ACubeGameCharacter::OnPhysicsInit()
{
	if (PhysicsAsset != nullptr)
	{
		GetMesh()->SetPhysicsAsset(PhysicsAsset, false);
		GetMesh()->bUpdateJointsFromAnimation = true;
		GetMesh()->SetGenerateOverlapEvents(true);
		// GetMesh()->bUpdateMeshWhenKinematic = true;
		// GetMesh()->bIncludeComponentLocationIntoBounds = true;
	}
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
	GetMesh()->SetSimulatePhysics(true);
	SetUpMovementConstraint();
	bPreventInput = false;
}

void ACubeGameCharacter::SetUpMovementConstraint()
{
	MovementPhysicsConstraint->SetConstrainedComponents(GetCapsuleComponent(), EName::None, GetMesh(), BodyName);
	// MovementPhysicsConstraint->AddRelativeLocation(FVector(0, 0, 14.5f));
	MovementPhysicsConstraint->SetLinearXLimit(LCM_Limited, XLimit);
	MovementPhysicsConstraint->SetLinearYLimit(LCM_Limited, XLimit);
	MovementPhysicsConstraint->SetLinearZLimit(LCM_Limited, ZLimit);

	// soft constraints
	MovementPhysicsConstraint->ConstraintInstance.SetSoftLinearLimitParams(true, Stiffness, Damping, Restitution, ContactDistance);
}

void ACubeGameCharacter::UpdateMovementConstraint(float DeltaSeconds)
{
	const double Speed = UKismetMathLibrary::VSize(GetVelocity());
	UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(Speed));

	// Spring for LinearLimit
	{
		float TargetLimit = UKismetMathLibrary::Sqrt(Speed) * UKismetMathLibrary::Loge(Speed + 1) / 4.0f;
		ConstraintSpring.bPrevTargetValid = false;
		XLimit = UKismetMathLibrary::FClamp(UKismetMathLibrary::FloatSpringInterp(XLimit, TargetLimit,
			ConstraintSpring, 20, 1, DeltaSeconds), 17.5, 100.0f);
		YLimit = XLimit;
		ZLimit = XLimit;
		MovementPhysicsConstraint->SetLinearXLimit(LCM_Limited, XLimit);
		MovementPhysicsConstraint->SetLinearYLimit(LCM_Limited, YLimit);
		MovementPhysicsConstraint->SetLinearZLimit(LCM_Limited, ZLimit);
		UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(XLimit));
	}

	// Spring for SoftLinearLimit
	 {

		StiffnessSpring.bPrevTargetValid = false;
		DampingSpring.bPrevTargetValid = false;
	 	const float TargetStiffness = UKismetMathLibrary::Exp(-Speed)*100 + 20;
		const float TargetDamping = Speed * Speed / 1000;
		Stiffness = UKismetMathLibrary::FloatSpringInterp(Stiffness, TargetStiffness, StiffnessSpring, 20, 1, DeltaSeconds);
		Damping = UKismetMathLibrary::FloatSpringInterp(Damping, TargetDamping, DampingSpring, 20, 1, DeltaSeconds);
	 	MovementPhysicsConstraint->ConstraintInstance.SetSoftLinearLimitParams(true, Stiffness, Damping, Restitution, ContactDistance);
		UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_DoubleToString(Stiffness));

	 }
}

void ACubeGameCharacter::MoveForward(float Value)
{
	if (!bPreventInput)
	{
		ForwardSpring.bPrevTargetValid = false;
		ForwardValue = UKismetMathLibrary::FloatSpringInterp(ForwardValue, Value, ForwardSpring, 20, 1, GetWorld()->GetDeltaSeconds());
		ACubeGameCharacterBase::MoveForward(ForwardValue);
	}
}

void ACubeGameCharacter::MoveRight(float Value)
{
	if (!bPreventInput)
	{
		RightSpring.bPrevTargetValid = false;
		RightValue = UKismetMathLibrary::FloatSpringInterp(RightValue, Value, RightSpring, 20, 1, GetWorld()->GetDeltaSeconds());

		ACubeGameCharacterBase::MoveRight(RightValue);
	}
}

void ACubeGameCharacter::BeginSprint()
{
	SprintTimelineComponent->Play();
}

void ACubeGameCharacter::EndSprint()
{
	SprintTimelineComponent->Reverse();
}

void ACubeGameCharacter::OnSprintTimelineTick(float Value)
{
	GetCharacterMovement()->MaxWalkSpeed = Value;
}

// TODO Land BUG
void ACubeGameCharacter::Jump()
{
	if (!bPreventInput)
	{
		if (UCubeAnimInstance* CubeAnimInstance = Cast<UCubeAnimInstance>(AnimInstance))
		{
			if (CubeAnimInstance->JumpCount < 2)
			{
				CubeAnimInstance->JumpCount += 1;
				CubeAnimInstance->bIsJumping = true;
				if (CubeAnimInstance->JumpCount == 1)
				{
					GetWorldTimerManager().SetTimer(JumpTimerHandle, [this]()
					{
						LaunchCharacter(FVector(0.0f, 0.0f, 600.0f), false, false);
						GetWorldTimerManager().ClearTimer(JumpTimerHandle);

					}, .2f, false);
				}
				else
				{
					LaunchCharacter(FVector(0.0f, 0.0f, 600.0f), false, false);
					// GetWorld()->GetTimerManager().SetTimer(JumpTimeHandle, [this]()
					// {
					// 	GetWorld()->GetTimerManager().ClearTimer(JumpTimeHandle);
					//
					// }, .2f, false);
				}
			}
		}
		else
		{
			Super::Jump();
		}

	}
}

void ACubeGameCharacter::Landed(const FHitResult& Hit)
{
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
	if (!bPreventInput)
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
	GetCameraBoom()->SetRelativeLocation(FVector(0, 0, 50.0f));
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
	
	GetCameraBoom()->SetRelativeLocation(FVector(0, 0, 10.0f));
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






