// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Third_Person_RPG/Data/SkillData.h"
#include "TPRWeapon.generated.h"

UENUM(BlueprintType)
enum class WeaponType : uint8
{
	Sword,
	Bow,
	Staff,
};

UCLASS()
class THIRD_PERSON_RPG_API ATPRWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPRWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMesh* SkeletalMeshAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment")
	FVector RelativeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment")
	FRotator RelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment")
	FVector RelativeScale = FVector(1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	USkillData* SkillData;

	UPROPERTY(EditAnywhere, Category = ComboData, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMMComboActionData> ComboData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UBoxComponent* HitBox;

	UFUNCTION(BlueprintCallable)
	void EnableHitBox();

	UFUNCTION(BlueprintCallable)
	void DisableHitBox();

	UFUNCTION()
	void OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
