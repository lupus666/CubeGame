// Fill out your copyright notice in the Description page of Project Settings.


#include "CubePlayerState.h"

#include "CubeGameStateBase.h"
#include "Environment/Portal.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Materials/MaterialParameterCollection.h"

ACubePlayerState::ACubePlayerState()
{
	// PrimaryActorTick.bCanEverTick = true;
}

void ACubePlayerState::BeginPlay()
{
	Super::BeginPlay();

	PortalStates.Init(false, Cast<ACubeGameStateBase>(GetWorld()->GetGameState())->GetPortalCount());
}

void ACubePlayerState::UpdatePortalState()
{
	if (ACubeGameStateBase* CubeGameStateBase = Cast<ACubeGameStateBase>(GetWorld()->GetGameState()))
	{
		// TArray<UMaterialParameterCollection* > PortalCollections = CubeGameStateBase->GetPortalCollections();
		// for (int i = 0; i < PortalCollections.Num(); i++)
		// {
		// 	const bool bVisibility = UKismetMaterialLibrary::GetScalarParameterValue(this, PortalCollections[i], FName("Visibility")) == 1.0;
		// 	PortalStates[i] = bVisibility;
		// 	if (bVisibility)
		// 	{
		// 		InPortals.Add(i + 1);
		// 	}
		// 	else
		// 	{
		// 		InPortals.Remove(i + 1);
		// 	}
		// }
		
		UMaterialParameterCollection* PortalCollectionSingle = CubeGameStateBase->GetPortalCollectionSingle();
		for (int i = 0; i < PortalStates.Num(); i++)
		{
			bool bVisibility = UKismetMaterialLibrary::GetScalarParameterValue(this, PortalCollectionSingle,
				FName(FString("Visibility") + UKismetStringLibrary::Conv_IntToString(i + 1))) == 1.0;
			PortalStates[i] = bVisibility;
			if (bVisibility)
			{
				InPortals.Add(i + 1);
			}
			else
			{
				InPortals.Remove(i + 1);
			}
		}
	}
}

