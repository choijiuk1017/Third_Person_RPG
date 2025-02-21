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
	//스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArmComp;

	//카메라 컴포넌트
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
	//구르기 애니메이션 몽타주
	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> RollMontage;

	//콤보 공격 애니메이션 몽타주
	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> BasicComboMontage;

	//Data Section
	//콤보 공격 데이터
	UPROPERTY(EditAnywhere, Category = ComboData, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMMComboActionData> BasicComboData;




	//Function Section
	//기본 이동 함수
	void BasicMove(const FInputActionValue& Value);
	//카메라 시점 함수
	void BasicLook(const FInputActionValue& Value);

	void BeginSprint();
	void EndSprint();
	//구르기 시작 함수
	void RollStart();
	//구르기 종료 함수
	void RollEnd(class UAnimMontage* Montage, bool IsEnded);
	//기본 공격 함수
	void BasicAttack();
	//콤보 시작 함수
	void ComboStart();
	//콤보 종료 함수
	void ComboEnd(class UAnimMontage* Montage, bool IsEnded);
	//콤보가 이어지는 지 확인하는 함수
	void ComboCheck();
	//콤보 체크 호출 시간 설정 함수
	void SetComboTimer();

	//공격 체크 함수, 인터페이스에서 상속 받음
	virtual void BaseAttackCheck() override;

	//Variable Section
	//구르기 확인 변수
	uint8 bIsRoll : 1;
	//콤보에 사용될 타이머 변수
	FTimerHandle ComboTimerHandle;
	//현재 콤보 진행 수
	int32 CurrentComboCount;
	//콤보 입력 판별
	uint8 bHasComboInput : 1;
	//공격 중 구르기, 구르기 중 공격 등 다른 모션을 막기 위한 변수
	uint8 bIsAttacking : 1;


private:



};
