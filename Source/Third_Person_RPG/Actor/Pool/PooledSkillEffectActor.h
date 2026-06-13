#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PooledSkillEffectActor.generated.h"

class UParticleSystem;
class UParticleSystemComponent;

UCLASS()
class THIRD_PERSON_RPG_API APooledSkillEffectActor : public AActor
{
	GENERATED_BODY()

public:
	APooledSkillEffectActor();

	void ActivateEffect(UParticleSystem* InEffect, const FVector& Location, const FRotator& Rotation);
	void DeactivateEffect();

	bool IsInUse() const;
	UParticleSystem* GetEffectTemplate() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ParticleComponent;

	UPROPERTY()
	UParticleSystem* EffectTemplate;

	bool bInUse = false;

	FTimerHandle DeactivateTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Pool")
	float LifeTime = 2.0f;
};