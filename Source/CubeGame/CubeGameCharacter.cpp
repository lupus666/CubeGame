// Copyright Epic Games, Inc. All Rights Reserved.

#include "CubeGameCharacter.h"

#include "CubeAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"


//////////////////////////////////////////////////////////////////////////
// ACubeGameCharacter

ACubeGameCharacter::ACubeGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(11, 11);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 120.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PhysicalAnimationComponent = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicalAnimation"));

	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

}

void ACubeGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Set Sprint Timeline
	if (SprintCurve)
	{
		FOnTimelineFloat SprintTimelineFloat;
		SprintTimelineFloat.BindUFunction(this, TEXT("OnSprintTimelineTick"));
		TimelineComponent->AddInterpFloat(SprintCurve, SprintTimelineFloat);
	}

	// Init Animation
	UAnimInstance* AnimInstance = (GetMesh())? GetMesh()->GetAnimInstance() : nullptr;
	if (InitMontage && AnimInstance)
	{
		PreventInput = true;
		PlayAnimMontage(InitMontage);
		FOnMontageEnded OnInitMontageEnded;
		OnInitMontageEnded.BindUObject(this, &ACubeGameCharacter::OnPhysicsInit);
		AnimInstance->Montage_SetEndDelegate(OnInitMontageEnded);
		
		CubeAnimInstance = Cast<UCubeAnimInstance>(AnimInstance);
	}
	else
	{
		PreventInput = false;
	}

	// Set Animation
	if (MMBAnim)
	{
		if (CubeAnimInstance != nullptr)
		{
			CubeAnimInstance->MMBAnim = MMBAnim;
		}
	}
	if (MountAnim)
	{
		if (CubeAnimInstance != nullptr)
		{
			CubeAnimInstance->MMBAnim = MMBAnim;
		}
	}

	// Set Material


	// Set UI Widget

	
	// Set Niagara
	
	
}


void ACubeGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACubeGameCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACubeGameCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACubeGameCharacter::EndSprint);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACubeGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACubeGameCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ACubeGameCharacter::BeginTighten);
	// PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ACubeGameCharacter::BeginTighten);
	//
	// PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ACubeGameCharacter::BeginRelax);
	// PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &ACubeGameCharacter::BeginRelax);
}

void ACubeGameCharacter::OnPhysicsInit(UAnimMontage* Montage, bool bInterrupted)
{
	UKismetSystemLibrary::Delay(this, 0.5f, FLatentActionInfo());
	
	if (PhysicsAsset != nullptr)
	{
		GetMesh()->SetPhysicsAsset(PhysicsAsset);
		PhysicalAnimationComponent->SetSkeletalMeshComponent(GetMesh());
		FPhysicalAnimationData PhysicalAnimationData;
		PhysicalAnimationData.bIsLocalSimulation = false;
		PhysicalAnimationData.OrientationStrength = 1000.f;
		PhysicalAnimationData.AngularVelocityStrength = 15.f;
		PhysicalAnimationData.PositionStrength = 1000.f;
		PhysicalAnimationData.VelocityStrength = 30.f;
		
		PhysicalAnimationComponent->ApplyPhysicalAnimationSettingsBelow(BodyName, PhysicalAnimationData);
		GetMesh()->SetAllBodiesBelowSimulatePhysics(BodyName, true, false);
		PreventInput = false;
	}
	
}

void ACubeGameCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !PreventInput)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACubeGameCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) && !PreventInput)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACubeGameCharacter::BeginSprint()
{
	TimelineComponent->Play();
}

void ACubeGameCharacter::EndSprint()
{
	TimelineComponent->Reverse();
}

void ACubeGameCharacter::OnSprintTimelineTick(float Value)
{
	GetCharacterMovement()->MaxWalkSpeed = Value;
}


//TODO DEBUG
void ACubeGameCharacter::Jump()
{
	if (!PreventInput)
	{
		if (CubeAnimInstance != nullptr)
		{
			if (CubeAnimInstance->JumpCount < 2)
			{
				CubeAnimInstance->JumpCount += 1;
				CubeAnimInstance->bIsJumping = true;
				if (CubeAnimInstance->JumpCount == 1)
				{
					UKismetSystemLibrary::Delay(this, .2f, FLatentActionInfo());
					LaunchCharacter(FVector(0.0f, 0.0f, 600.0f), false, false);
				}
				else
				{
					LaunchCharacter(FVector(0.0f, 0.0f, 600.0f), false, false);
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
	if (CubeAnimInstance != nullptr)
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
	
}

void ACubeGameCharacter::EndTighten()
{
	
}

void ACubeGameCharacter::BeginRelax()
{
	
}

void ACubeGameCharacter::EndRelax()
{
	
}




