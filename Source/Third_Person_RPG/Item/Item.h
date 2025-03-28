// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"
#include "Item.generated.h"

UCLASS()
class THIRD_PERSON_RPG_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	// 블루프린트에서 세팅할 무기 종류
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemBox")
	TSubclassOf<ATPRWeapon> WeaponClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USkeletalMeshComponent* BoxMesh;

	// 플레이어가 범위에 들어왔을 때 호출되는 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:

};
