// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ButtonInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UButtonInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUBEGAME_API IButtonInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Activate(bool bActivate);
	// virtual void Activate_Implementation(bool bActivate) = 0;
};
