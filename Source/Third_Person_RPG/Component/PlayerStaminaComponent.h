// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStaminaComponent.generated.h"

class APlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRD_PERSON_RPG_API UPlayerStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerStaminaComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APlayerCharacter* OwnerPlayer = nullptr;

public:
	void InitializeStaminaComponent(APlayerCharacter* InOwner);

	bool HasStamina(int32 Amount) const;
	bool TryConsumeStamina(int32 Amount);
	void ConsumeStamina(int32 Amount);
	void RestoreStaminaTick(int32 AmountPerTick);

	void StaminaRegenTick(float DeltaSeconds);
	void SprintStaminaTick(float DeltaSeconds);

	int32 GetStaminaRegenPerSecond() const;

	bool HasFP(int32 Amount) const;
	bool TryConsumeFP(int32 Amount);
	void ConsumeFP(int32 Amount);
};
