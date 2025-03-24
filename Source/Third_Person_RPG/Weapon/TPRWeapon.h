// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	void EquipWeapon(ACharacter* Player);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPoseableMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> WeaponCollision;

	WeaponType WeaponType;

	FName BaseSocketName;

	UFUNCTION()
	void OnWeaponOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
