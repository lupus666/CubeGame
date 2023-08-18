// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CubePlayerController.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCameraOccludedActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	const AActor* Actor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UMaterialInterface*> Materials;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsOccluded;
};

UCLASS()
class CUBEGAME_API ACubePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACubePlayerController();

	void UpdateCharacter();
	
	UFUNCTION(BlueprintCallable)
	void SyncOccludedActors();

	UFUNCTION()
	void PauseGame();

	UFUNCTION()
	void ReturnGame();

	UFUNCTION()
	void MainMenu();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> PauseWidgetClass;

	UPROPERTY()
	class UUserWidget* PauseWidget;

	UPROPERTY()
	class UButton* ReturnGameButton;

	UPROPERTY()
	class UButton* MainMenuButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Occlusion", meta=(ClampMin="0.1", ClampMax="10.0") )
	float CapsulePercentageForTrace;
  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Occlusion")
	UMaterialInterface* FadeMaterial;
 
	UPROPERTY(BlueprintReadWrite, Category="Camera Occlusion")
	class USpringArmComponent* ActiveSpringArm;
 
	UPROPERTY(BlueprintReadWrite, Category="Camera Occlusion")
	class UCameraComponent* ActiveCamera;
 
	UPROPERTY(BlueprintReadWrite, Category="Camera Occlusion")
	class UCapsuleComponent* ActiveCapsuleComponent;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Occlusion")
	bool IsOcclusionEnabled;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Occlusion")
	bool DebugLineTraces;
  
private:
	UPROPERTY()
	TMap<const AActor*, FCameraOccludedActor> OccludedActors;
  
	bool HideOccludedActor(const AActor* Actor);
	
	void OnHideOccludedActor(const FCameraOccludedActor& OccludedActor) const;
	
	void ShowOccludedActor(FCameraOccludedActor& OccludedActor);
	
	void OnShowOccludedActor(const FCameraOccludedActor& OccludedActor) const;
	
	void ForceShowOccludedActors();
 
	inline bool ShouldCheckCameraOcclusion() const
	{
		return IsOcclusionEnabled && FadeMaterial && ActiveCamera && ActiveCapsuleComponent;
	}

};