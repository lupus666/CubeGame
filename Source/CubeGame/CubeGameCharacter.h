// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "CubeGameCharacter.generated.h"


UCLASS(config=Game)
class ACubeGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicalAnimationComponent* PhysicalAnimationComponent;

public:
	ACubeGameCharacter();

	UPROPERTY()
	class UTimelineComponent* TimelineComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCurveFloat* SprintCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimMontage* InitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimSequenceBase* MountAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimSequenceBase* MMBAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UPhysicsAsset* PhysicsAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCubeAnimInstance* CubeAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BodyName = "Pelvis";
	
protected:

	void OnPhysicsInit(UAnimMontage* Montage, bool bInterrupted);
	
	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginSprint();

	void EndSprint();
	
	UFUNCTION()
	void OnSprintTimelineTick(float Value);

	virtual void Jump() override;

	virtual void Landed(const FHitResult& Hit) override;

	void BeginTighten();

	void EndTighten();
	
	void BeginRelax();

	void EndRelax();
	
	bool PreventInput;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

