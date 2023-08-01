// Fill out your copyright notice in the Description page of Project Settings.


#include "Pinhole.h"

#include "CubeGameCharacter.h"
#include "CyberCube.h"
#include "BaseGizmos/GizmoElementArrow.h"
#include "Components/CapsuleComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
APinhole::APinhole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
}

// Called when the game starts or when spawned
void APinhole::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APinhole::BeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &APinhole::EndOverlap);
}

// Called every frame
void APinhole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//TODO Debug
void APinhole::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACyberCube* CyberCube = Cast<ACyberCube>(OtherActor))
	{
		if (UBoxComponent* Box = Cast<UBoxComponent>(OtherComp))
		{
			UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_FloatToString(CyberCube->GetCurrentRelaxRate()));
			if (CyberCube->GetCurrentRelaxRate() < CyberCube->RelaxThreshold && CyberCube->GetCube()->IsSimulatingPhysics(CyberCube->GetBodyName()))
			{
				CyberCube->GetCube()->GetBodyInstance(CyberCube->GetBodyName())->SetInstanceSimulatePhysics(false, false,true);
				CyberCube->SetCurrentRelaxRate(CyberCube->RelaxThreshold);
			}
			// CyberCube->GetCube()->SetSimulatePhysics(false);
			// CyberCube->GetCube()->SetAllBodiesBelowSimulatePhysics(CyberCube->GetBodyName(), true, false);
			UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_FloatToString(CyberCube->GetCurrentRelaxRate()));

			UKismetSystemLibrary::PrintString(this, "Begin");
		}
	}else if(ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(OtherActor))
	{
		if (UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(OtherComp))
		{
			UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_FloatToString(CubeGameCharacter->GetCurrentRelaxRate()));

			if (CubeGameCharacter->GetCurrentRelaxRate() <= CubeGameCharacter->RelaxThreshold && CubeGameCharacter->GetMesh()->IsSimulatingPhysics(CubeGameCharacter->GetBodyName()))
			{
				CubeGameCharacter->GetMesh()->GetBodyInstance(CubeGameCharacter->GetBodyName())->SetInstanceSimulatePhysics(false, false,true);
				// CubeGameCharacter->SetCurrentRelaxRate(CubeGameCharacter->RelaxThreshold);
				// CubeGameCharacter->GetMesh()->SetRelativeLocation(FVector(0, 0, -3));
			}
			UKismetSystemLibrary::PrintString(this, UKismetStringLibrary::Conv_FloatToString(CubeGameCharacter->GetCurrentRelaxRate()));

			UKismetSystemLibrary::PrintString(this, "Begin");

		}
		
	}
}

void APinhole::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (ACyberCube* CyberCube = Cast<ACyberCube>(OtherActor))
	{
		if (UBoxComponent* Box = Cast<UBoxComponent>(OtherComp))
		{
			if (!CyberCube->GetCube()->IsSimulatingPhysics(CyberCube->GetBodyName()))
			{
				// CyberCube->GetCube()->GetBodyInstance(CyberCube->GetBodyName())->SetInstanceSimulatePhysics(true, false, true);
				CyberCube->GetCube()->SetSimulatePhysics(true);

			}
			// CyberCube->GetCube()->SetSimulatePhysics(true);
			UKismetSystemLibrary::PrintString(this, "End");
		}
	}else if (ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(OtherActor))
	{
		if (!CubeGameCharacter->GetMesh()->IsSimulatingPhysics(CubeGameCharacter->GetBodyName()))
		{
			if (UBoxComponent* Box = Cast<UBoxComponent>(OtherComp))
			{
				CubeGameCharacter->GetMesh()->SetSimulatePhysics(true);
				UKismetSystemLibrary::PrintString(this, "End");
			}
		}
	}
}

