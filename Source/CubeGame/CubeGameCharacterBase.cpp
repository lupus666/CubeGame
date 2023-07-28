// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeGameCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputCoreTypes.h"
#include "Components/ArrowComponent.h"
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
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; 

}

// Called when the game starts or when spawned
void ACubeGameCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACubeGameCharacterBase::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACubeGameCharacterBase::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f))
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
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ACubeGameCharacterBase::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ACubeGameCharacterBase::ZoomOut);
}

