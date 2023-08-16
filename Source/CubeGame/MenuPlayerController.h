// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CUBEGAME_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> MenuWidget;

	UPROPERTY(EditAnywhere)
	FName StartLevelName = "LevelCubeWorld";

	UPROPERTY(EditAnywhere)
	FName DemoLevelName = "DemoGame";
	
	UPROPERTY()
	class UUserWidget* CurrentWidget;

	UPROPERTY()
	class UButton* NewGameButton;

	UPROPERTY()
	class UButton* DemoGameButton;

	UPROPERTY()
	class UButton* QuitGameButton;

public:
	UFUNCTION()
	void NewGame();

	UFUNCTION()
	void DemoGame();

	UFUNCTION()
	void QuitGame();
};
