// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeMountCharacter.h"

#include "CubeAnimInstance.h"
#include "CubeGameCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputCoreTypes.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"


ACubeMountCharacter::ACubeMountCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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

	GetCameraBoom()->bEnableCameraLag = true;
	GetCameraBoom()->CameraLagMaxDistance = 10.0f;

	TightenTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TightenTimelineComponent"));
	DilationTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DilationTimelineComponent"));
}

// Called when the game starts or when spawned
void ACubeMountCharacter::BeginPlay()
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
	
}

void ACubeMountCharacter::OnMount(ACharacter* MountCharacter)
{
	CubeMountCharacter = Cast<ACubeMountCharacter>(MountCharacter);
	if (ACubeGameCharacter* CubeCharacter = Cast<ACubeGameCharacter>(MountCharacter))
	{
		// CubeCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		// CubeCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		CubeCharacter->GetRootComponent()->SetWorldLocation(CubeCharacter->GetMountLocation(), true);
		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
		FName SocketName = UKismetStringLibrary::Conv_StringToName(UKismetStringLibrary::BuildString_Name("", "", CubeCharacter->GetMountBoneName(), "Socket"));
		UKismetSystemLibrary::PrintString(this, SocketName.ToString());
		if (CubeCharacter->AttachToComponent(GetMesh(), AttachmentTransformRules, SocketName))
		{
			UGameplayStatics::GetPlayerController(this, 0)->Possess(this);
			if (UCubeAnimInstance* CubeAnimInstance = Cast<UCubeAnimInstance>(CubeCharacter->AnimInstance))
			{
				CubeAnimInstance->bIsOnMount = true;
			}
			
		};
	}
}

void ACubeMountCharacter::UnMount()
{
	if (ACubeGameCharacter* CubeCharacter = Cast<ACubeGameCharacter>(CubeMountCharacter))
	{
		CubeCharacter->bIsMounted = false;
		if (UCubeAnimInstance* CubeAnimInstance = Cast<UCubeAnimInstance>(CubeCharacter->AnimInstance))
		{
			CubeAnimInstance->bIsOnMount = false;
		}
		// CubeCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// CubeCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, true);
		CubeCharacter->DetachFromActor(DetachmentTransformRules);
		CubeCharacter->GetRootComponent()->SetWorldLocation(GetRootComponent()->GetComponentLocation() + FVector(0.0, 0.0, 50.0f) + GetActorForwardVector() * -10.0f);

		UGameplayStatics::GetPlayerController(this, 0)->Possess(Cast<APawn>(CubeCharacter));
		CubeCharacter->SpawnDefaultController();
		CubeMountCharacter = nullptr;
	}
}

void ACubeMountCharacter::TimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 0.2);
	CustomTimeDilation = 1.0 / 0.2;
	// if (bTimeDilation)
	// {
	// 	DilationTimelineComponent->Reverse();
	// 	bTimeDilation = false;
	// }
	// else
	// {
	// 	DilationTimelineComponent->Play();
	// 	bTimeDilation = true;
	// }
}

void ACubeMountCharacter::OnDilationTimelineTick(float Value)
{
	UGameplayStatics::SetGlobalTimeDilation(this, Value);
	CustomTimeDilation = 1.0 / Value;
}

void ACubeMountCharacter::BeginTighten()
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

void ACubeMountCharacter::EndTighten()
{
	TightenTimelineComponent->Stop();
}

void ACubeMountCharacter::BeginRelax()
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

void ACubeMountCharacter::EndRelax()
{
	TightenTimelineComponent->Stop();
}

void ACubeMountCharacter::OnTightenTimelineTick(float Value)
{
	CurrentRelaxRate = Value;
}

// Called every frame
void ACubeMountCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CubeMountCharacter)
	{
		CubeMountCharacter->CurrentRelaxRate = CurrentRelaxRate;
	}
}

// Called to bind functionality to input
void ACubeMountCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACubeMountCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACubeMountCharacter::MoveRight);
	
	// PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	// PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACubeGameCharacterBase::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACubeGameCharacterBase::LookUp);
	
	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ACubeMountCharacter::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ACubeMountCharacter::ZoomOut);

	PlayerInputComponent->BindAction("UnMount", IE_Pressed, this, &ACubeMountCharacter::UnMount);
	PlayerInputComponent->BindAction("Dilation", IE_Pressed, this, &ACubeMountCharacter::TimeDilation);

	PlayerInputComponent->BindAction("Tighten", IE_Pressed, this, &ACubeMountCharacter::BeginTighten);
	PlayerInputComponent->BindAction("Tighten", IE_Released, this, &ACubeMountCharacter::EndTighten);
	PlayerInputComponent->BindAction("Relax", IE_Pressed, this, &ACubeMountCharacter::BeginRelax);
	PlayerInputComponent->BindAction("Relax", IE_Released, this, &ACubeMountCharacter::EndRelax);
}

