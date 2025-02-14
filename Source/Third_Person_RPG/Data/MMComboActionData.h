// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MMComboActionData.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UMMComboActionData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UMMComboActionData();

public:
	//��Ÿ�� ���� �̸�
	UPROPERTY(EditAnywhere, Category = Name)
	FString SectionPrefix;

	//��� �ӵ�
	UPROPERTY(EditAnywhere, Category = ComboData)
	float FrameRate;

	//�ִ� ���� �޺� ��
	UPROPERTY(EditAnywhere, Category = ComboData)
	uint8 MaxComboCount;

	//�޺��� ���� �޺��� �Ѿ�� ���� �Է� ������ ����
	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> ComboFrame;
};
