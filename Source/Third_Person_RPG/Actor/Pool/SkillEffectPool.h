#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillEffectPool.generated.h"

class APooledSkillEffectActor;
class USkillData;
class UParticleSystem;

UCLASS()
class THIRD_PERSON_RPG_API ASkillEffectPool : public AActor
{
	GENERATED_BODY()

public:
	ASkillEffectPool();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "Pool")
	TSubclassOf<APooledSkillEffectActor> PooledEffectClass;

	UPROPERTY(EditAnywhere, Category = "Pool")
	int32 PoolSize = 20;

	UFUNCTION(BlueprintCallable)
	void PlaySkillEffect(AActor* OwnerActor, USkillData* SkillData);

private:
	UPROPERTY()
	TArray<APooledSkillEffectActor*> EffectPool;

	APooledSkillEffectActor* GetAvailableEffect();
	FVector CalculateSpawnLocation(AActor* OwnerActor, USkillData* SkillData) const;
};