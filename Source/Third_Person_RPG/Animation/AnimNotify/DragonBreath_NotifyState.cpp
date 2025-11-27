// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Animation/AnimNotify/DragonBreath_NotifyState.h"
#include "Third_Person_RPG/Character/BossDragon.h"
#include "Kismet/GameplayStatics.h"

void UDragonBreath_NotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    ABossDragon* Dragon = Cast<ABossDragon>(MeshComp->GetOwner());
    if (!Dragon) return;

    Dragon->StartBreath();
}

void UDragonBreath_NotifyState::NotifyTick(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    float FrameDeltaTime)
{
    ABossDragon* Dragon = Cast<ABossDragon>(MeshComp->GetOwner());
    if (!Dragon) return;

    Dragon->ApplyBreathDamage();
}

void UDragonBreath_NotifyState::NotifyEnd(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation)
{
    ABossDragon* Dragon = Cast<ABossDragon>(MeshComp->GetOwner());
    if (!Dragon) return;

    Dragon->EndBreath();
}