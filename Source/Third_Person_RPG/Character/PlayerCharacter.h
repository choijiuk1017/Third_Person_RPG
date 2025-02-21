// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "InputActionValue.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Third_Person_RPG/Interface/AnimationAttackInterface.h"

#include "PlayerCharacter.generated.h"



UCLASS()
class THIRD_PERSON_RPG_API APlayerCharacter : public ACharacter, public IAnimationAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Component Section
	//������ �� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArmComp;

	//ī�޶� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraComp;



	//Input Section
	UPROPERTY(VisibleAnywhere, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> IMC_Basic;

	UPROPERTY(VisibleAnywhere, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_BasicMove;

	UPROPERTY(VisibleAnywhere, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_BasicLook;

	UPROPERTY(VisibleAnywhere, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Sprint;

	UPROPERTY(VisibleAnywhere, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Roll;

	UPROPERTY(VisibleAnywhere, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> IA_Attack;




	//Montage Section
	//������ �ִϸ��̼� ��Ÿ��
	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> RollMontage;

	//�޺� ���� �ִϸ��̼� ��Ÿ��
	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> BasicComboMontage;

	//Data Section
	//�޺� ���� ������
	UPROPERTY(EditAnywhere, Category = ComboData, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMMComboActionData> BasicComboData;




	//Function Section
	//�⺻ �̵� �Լ�
	void BasicMove(const FInputActionValue& Value);
	//ī�޶� ���� �Լ�
	void BasicLook(const FInputActionValue& Value);

	void BeginSprint();
	void EndSprint();
	//������ ���� �Լ�
	void RollStart();
	//������ ���� �Լ�
	void RollEnd(class UAnimMontage* Montage, bool IsEnded);
	//�⺻ ���� �Լ�
	void BasicAttack();
	//�޺� ���� �Լ�
	void ComboStart();
	//�޺� ���� �Լ�
	void ComboEnd(class UAnimMontage* Montage, bool IsEnded);
	//�޺��� �̾����� �� Ȯ���ϴ� �Լ�
	void ComboCheck();
	//�޺� üũ ȣ�� �ð� ���� �Լ�
	void SetComboTimer();

	//���� üũ �Լ�, �������̽����� ��� ����
	virtual void BaseAttackCheck() override;

	//Variable Section
	//������ Ȯ�� ����
	uint8 bIsRoll : 1;
	//�޺��� ���� Ÿ�̸� ����
	FTimerHandle ComboTimerHandle;
	//���� �޺� ���� ��
	int32 CurrentComboCount;
	//�޺� �Է� �Ǻ�
	uint8 bHasComboInput : 1;
	//���� �� ������, ������ �� ���� �� �ٸ� ����� ���� ���� ����
	uint8 bIsAttacking : 1;


private:



};
