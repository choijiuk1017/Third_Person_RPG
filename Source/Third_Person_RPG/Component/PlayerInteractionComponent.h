// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInteractionComponent.generated.h"

class APlayerCharacter;
class AItem;
class ASavePoint;
class ANPC;
class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRD_PERSON_RPG_API UPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerInteractionComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APlayerCharacter* OwnerPlayer = nullptr;

public:
	void InitializeInteractionComponent(APlayerCharacter* InOwner);

	void Interact();

	void SetOverlappingItem(AItem* Item);
	void ReSetOverlappingItem();

	void SetCurrentNPC(ANPC* InNPC);

	void OnEquipAnimationEnd(UAnimMontage* Montage, bool bInterrupted);
		
};
