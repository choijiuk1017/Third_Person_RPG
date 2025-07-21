// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/EnemyCharacter.h"

#include "Third_Person_RPG/Character/EnemyAIController.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/EngineTypes.h"


#define CHANNEL_ACTION ECollisionChannel::ECC_GameTraceChannel2

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyAIController::StaticClass();
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemyCharacter::BaseAttackCheck()
{
	//충돌 결과를 반환하기 위한 배열
	TArray<FHitResult> OutHitResults;

	//충돌 탐지를 위한 시작 지점
	FVector Start = GetActorLocation() + (GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius());

	//충돌 탐지 끝 지점
	FVector End = Start + (GetActorForwardVector() * 70.0f);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	bool bHasHit = GetWorld()->SweepMultiByChannel(
		OutHitResults,
		Start,
		End,
		FQuat::Identity,
		CHANNEL_ACTION,
		FCollisionShape::MakeSphere(70.0f),
		Params
	);

	//공격 판정 시 데미지 처리 예정
	if (bHasHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Damaged"));
	}

	// Capsule 모양의 디버깅 체크
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = 70.0f * 0.5f;
	FColor DrawColor = bHasHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, 70.0f, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 3.0f);
}

void AEnemyCharacter::PlayHitReactMontage()
{
	if (HitReactMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage);
	}
}