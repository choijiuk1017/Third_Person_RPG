// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/LastBoss.h"
#include "Components/StaticMeshComponent.h"

ALastBoss::ALastBoss()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));


	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));


	//WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}