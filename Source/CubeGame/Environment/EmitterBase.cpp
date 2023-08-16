// Fill out your copyright notice in the Description page of Project Settings.


#include "EmitterBase.h"

#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AEmitterBase::AEmitterBase()
{
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow");
	ArrowComponent->SetupAttachment(RootComponent);
}

void AEmitterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AEmitterBase::Emitter()
{
	if (EmitterActorClass)
	{
		FVector Direction = UKismetMathLibrary::RandomUnitVectorInConeInRadians(ArrowComponent->GetForwardVector(), 0.1);
		FRotator Rotation = (Direction * Strength).Rotation();
		FTransform SpawnTransform = FTransform(UKismetMathLibrary::MakeTransform(ArrowComponent->GetComponentLocation(), Rotation));
		APortalActor* EmitterActor = Cast<APortalActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,
				EmitterActorClass, SpawnTransform));
		// EmitterActor->Initialize(this);
		UGameplayStatics::FinishSpawningActor(EmitterActor, SpawnTransform);
	}
}
