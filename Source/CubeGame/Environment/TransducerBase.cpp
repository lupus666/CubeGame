// Fill out your copyright notice in the Description page of Project Settings.


#include "TransducerBase.h"

#include "Components/BoxComponent.h"
#include "CubeGame/ButtonInterface.h"

// Sets default values
ATransducerBase::ATransducerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("Box");
	BoxComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATransducerBase::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ATransducerBase::BeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ATransducerBase::EndOverlap);

}

// Called every frame
void ATransducerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATransducerBase::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		if (TriggerActorClass != nullptr)
		{
			if (OtherActor->GetClass() == TriggerActorClass)
			{
				if (IsActorValid(OtherActor))
				{
					if (IButtonInterface* ButtonInterface = Cast<IButtonInterface>(ActivateActor))
					{
						IButtonInterface::Execute_Activate(ActivateActor, true);
					}
				}
			}
		}
		if (TriggerActor == nullptr)
		{
			if (IsActorValid(OtherActor))
			{
				if (IButtonInterface* ButtonInterface = Cast<IButtonInterface>(ActivateActor))
				{
					IButtonInterface::Execute_Activate(ActivateActor, true);
				}
			}
		}
		if (OtherActor == TriggerActor)
		{
			if (IButtonInterface* ButtonInterface = Cast<IButtonInterface>(ActivateActor))
			{
				IButtonInterface::Execute_Activate(ActivateActor, true);
			}
		}
	}
}

void ATransducerBase::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor))
	{
		if (TriggerActorClass != nullptr)
		{
			if (OtherActor->GetClass() == TriggerActorClass)
			{
				if (IsActorValid(OtherActor))
				{
					if (IButtonInterface* ButtonInterface = Cast<IButtonInterface>(ActivateActor))
					{
						IButtonInterface::Execute_Activate(ActivateActor, true);
					}
				}
			}
		}
		if (TriggerActor == nullptr)
		{
			if (IsActorValid(OtherActor))
			{
				if (IButtonInterface* ButtonInterface = Cast<IButtonInterface>(ActivateActor))
				{
					IButtonInterface::Execute_Activate(ActivateActor, false);
				}
			}
		}
		if (OtherActor == TriggerActor)
		{
			if (IButtonInterface* ButtonInterface = Cast<IButtonInterface>(ActivateActor))
			{
				IButtonInterface::Execute_Activate(ActivateActor, false);
			}
		}
	}
}

