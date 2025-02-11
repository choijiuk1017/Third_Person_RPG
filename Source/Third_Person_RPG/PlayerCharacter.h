// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MMComboActionData.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"


UCLASS()
class THIRD_PERSON_RPG_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	uint8 bIsRoll : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> RollMontage;

	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> BasicComboMontage;

	UPROPERTY(EditAnywhere, Category = ComboData, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMMComboActionData> BasicComboData;


protected:
	void MoveForward(float InputAxis);

	void MoveRight(float InputAxis);

	void BeginSprint();

	void EndSprint();

	void RollStart();

	void RollEnd(class UAnimMontage* Montage, bool IsEnded);

	void BasicAttack();
	void ComboStart();
	void ComboEnd(class UAnimMontage* Montage, bool IsEnded);
	void ComboCheck();
	void SetComboTimer();

	FTimerHandle ComboTimerHandle;

	int32 CurrentComboCount;

	uint8 bHasComboInput : 1;


	uint8 bIsAttacking : 1;


private:


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
