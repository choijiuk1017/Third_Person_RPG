// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSavePointComponent.generated.h"

class APlayerCharacter;
class ASavePoint;
class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRD_PERSON_RPG_API UPlayerSavePointComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerSavePointComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APlayerCharacter* OwnerPlayer = nullptr;

public:
	void InitializeSavePointComponent(APlayerCharacter* InOwner);

	void SetOverlappingSavePoint(ASavePoint* SavePoint);
	void ReSetOverlappingSavePoint();

	void InteractingSavePoint(UAnimMontage* Montage, bool bInterrupted);
	void EndInteractSavePoint();
		
};
