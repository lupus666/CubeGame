// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeGameCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputCoreTypes.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

ACubeGameCharacterBase::ACubeGameCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// GetMesh()->DetachFromParent();
	// RootComponent = GetMesh();
	// GetArrowComponent()->SetupAttachment(RootComponent);
	// GetCharacterMovement()->UpdatedComponent = RootComponent;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 0.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;
	
}

void ACubeGameCharacterBase::RotateCameraToGravity(FVector GravityDirection)
{
	GravityDirection = GravityDirection.GetSafeNormal();
	if (!FMath::IsNearlyEqual(FVector::DotProduct(-GravityDirection, UpVector), 1.0))
	{
		float Angle = 0;
		FVector RotationAxis = FVector(0, 0, 0);
		AxisAngleBetween(UpVector, -GravityDirection, RotationAxis, Angle);
		if (Angle < MinGravityTurnAngle)
		{
			UpVector = -GravityDirection;
		}
		else
		{
			const float DeltaAngle = GravityTurnRate * GetWorld()->GetDeltaSeconds();
			if (DeltaAngle >= Angle)
			{
				UpVector = UKismetMathLibrary::RotateAngleAxis(UpVector, Angle, RotationAxis).GetSafeNormal();
			}
			else
			{
				UpVector = UKismetMathLibrary::RotateAngleAxis(UpVector, (Angle > 0.0) ? DeltaAngle: -1*DeltaAngle, RotationAxis).GetSafeNormal();
			}
		}
		
		FVector LastForward = CameraBoom->GetForwardVector();
		// Rotate Capsule
		{
			GetCapsuleComponent()->SetConstraintMode(EDOFMode::None);
			AxisAngleBetween(FVector(0, 0, 1), UpVector, RotationAxis, Angle);
			const FRotator Rotator = UKismetMathLibrary::RotatorFromAxisAndAngle(RotationAxis, Angle);
			// GetCapsuleComponent()->SetSimulatePhysics(false);
			GetCapsuleComponent()->SetWorldRotation(Rotator);
			GetCapsuleComponent()->SetConstraintMode(EDOFMode::Default);
			// GetCapsuleComponent()->SetSimulatePhysics(true);
		}
		// Rotate Camera
		{
			//TODO Debug
			// const FRotator Rotator = UKismetMathLibrary::InverseTransformRotation(GetCapsuleComponent()->GetComponentToWorld(), LastForward.Rotation());
			// const float Pitch = UKismetMathLibrary::ClampAngle(Rotator.Pitch, -LookUpLimit, LookUpLimit);
			// CameraBoom->SetRelativeRotation(FRotator(0.0f, Pitch, Rotator.Yaw));
		}
	}

	
}

void ACubeGameCharacterBase::AxisAngleBetween(FVector A, FVector B, FVector& Axis, float& Angle)
{
	A = A.GetSafeNormal();
	B = B.GetSafeNormal();
	float Degree = UKismetMathLibrary::DegAcos(FVector::DotProduct(A, B));
	if (FMath::IsNearlyEqual(Degree, 0.0f))
	{
		Axis = FVector(0, 0, 1);
		Angle = 0;
	}
	else
	{
		if (FMath::IsNearlyEqual(FMath::Abs(Degree), 180.0f, 0.1f))
		{
			if (FMath::IsNearlyEqual(FMath::Abs(A.X), 1.0))
			{
				Axis = FVector(0, 0, 1);
				Angle = Degree;
			}
			else
			{
				Axis = FVector::CrossProduct(A, FVector(1,0,0)).GetSafeNormal();
				Angle = Degree;
			}
		}
		else
		{
			Axis = FVector::CrossProduct(A, B).GetSafeNormal();
			Angle = Degree;
		}
	}
}

// Called when the game starts or when spawned
void ACubeGameCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACubeGameCharacterBase::MoveForward(float Value)
{
	// if ((Controller != nullptr) && (Value != 0.0f))
	// {
	// 	// find out which way is forward
	// 	const FRotator Rotation = Controller->GetControlRotation();
	// 	const FRotator YawRotation(0, Rotation.Yaw, 0);
	//
	// 	// get forward vector
	// 	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// 	AddMovementInput(Direction, Value);
	// }
	if (Value != 0.0f)
	{
		const FVector Direction = CameraBoom->GetForwardVector();
		AddMovementInput(Direction, Value);
	}
}

void ACubeGameCharacterBase::MoveRight(float Value)
{
	// if ( (Controller != nullptr) && (Value != 0.0f))
	// {
	// 	// find out which way is right
	// 	const FRotator Rotation = Controller->GetControlRotation();
	// 	const FRotator YawRotation(0, Rotation.Yaw, 0);
	//
	// 	// get right vector 
	// 	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	// 	// add movement in that direction
	// 	AddMovementInput(Direction, Value);
	// }
	if (Value != 0.0f)
	{
		const FVector Direction = CameraBoom->GetRightVector();
		AddMovementInput(Direction, Value);
	}
}

void ACubeGameCharacterBase::ZoomIn()
{
	CameraBoom->TargetArmLength = UKismetMathLibrary::Clamp(CameraBoom->TargetArmLength - 20.0f, 90.0f, 1000.0f);
	CameraBoom->SetRelativeLocation(FVector(0, 0, 10));
}

void ACubeGameCharacterBase::ZoomOut()
{
	CameraBoom->TargetArmLength = UKismetMathLibrary::Clamp(CameraBoom->TargetArmLength + 20.0f, 90.0f, 1000.0f);
	CameraBoom->SetRelativeLocation(FVector(0, 0, 10));
}

void ACubeGameCharacterBase::Turn(float Value)
{
	CameraBoom->AddRelativeRotation(UKismetMathLibrary::MakeRotator(0, 0, Value * TurnRate));
}

void ACubeGameCharacterBase::LookUp(float Value)
{
	FRotator Rotator = CameraBoom->GetRelativeRotation();
	float Pitch = UKismetMathLibrary::ClampAngle(-Value * LookUpRate + Rotator.Pitch, -LookUpLimit, LookUpLimit);
	CameraBoom->SetRelativeRotation(UKismetMathLibrary::MakeRotator(Rotator.Roll, Pitch, Rotator.Yaw));
}

// Called every frame
void ACubeGameCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACubeGameCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACubeGameCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACubeGameCharacterBase::MoveRight);
	
	// PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	// PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACubeGameCharacterBase::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACubeGameCharacterBase::LookUp);

	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ACubeGameCharacterBase::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ACubeGameCharacterBase::ZoomOut);
}

