// Fill out your copyright notice in the Description page of Project Settings.


#include "Pinhole.h"

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

void APinhole::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(OtherActor))
	{
		CubeGameCharacter->GetMesh()->SetSimulatePhysics(false);
		CubeGameCharacter->GetMesh()->SetAllBodiesBelowSimulatePhysics(CubeGameCharacter->GetBodyName(), true, false);
		UKismetSystemLibrary::PrintString(this, "Begin");

	}
}

void APinhole::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (ACubeGameCharacter* CubeGameCharacter = Cast<ACubeGameCharacter>(OtherActor))
	{
		CubeGameCharacter->GetMesh()->SetAllBodiesBelowSimulatePhysics(CubeGameCharacter->GetBodyName(), false, false);
		CubeGameCharacter->GetMesh()->SetSimulatePhysics(true);

		UKismetSystemLibrary::PrintString(this, "End");
	}
}

