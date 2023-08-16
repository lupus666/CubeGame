// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuPlayerController.h"
#include "Kismet/GameplayStatics.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (MenuWidget)
	{
		CurrentWidget = CreateWidget<UUserWidget>(this, MenuWidget);
		if (CurrentWidget)
		{
			NewGameButton = Cast<UButton>(CurrentWidget->GetWidgetFromName(FName("NewGameButton")));
			DemoGameButton = Cast<UButton>(CurrentWidget->GetWidgetFromName(FName("DemoGameButton")));
			QuitGameButton = Cast<UButton>(CurrentWidget->GetWidgetFromName(FName("QuitGameButton")));
			if (NewGameButton)
			{
				NewGameButton->OnClicked.AddDynamic(this, &AMenuPlayerController::NewGame);
			}
			if (DemoGameButton)
			{
				DemoGameButton->OnClicked.AddDynamic(this, &AMenuPlayerController::DemoGame);
			}
			if (QuitGameButton)
			{
				QuitGameButton->OnClicked.AddDynamic(this, &AMenuPlayerController::QuitGame);
			}
			CurrentWidget->AddToViewport();
			SetShowMouseCursor(true);
		}
	}
}

void AMenuPlayerController::NewGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), StartLevelName, false);
	SetShowMouseCursor(false);
}

void AMenuPlayerController::DemoGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), DemoLevelName, false);
	SetShowMouseCursor(false);
}

void AMenuPlayerController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, true);
}
