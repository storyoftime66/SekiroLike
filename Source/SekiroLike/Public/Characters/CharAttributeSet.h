// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "CharAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class SEKIROLIKE_API UCharAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCharAttributeSet();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Attributes")
	FGameplayAttributeData Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Attributes")
	FGameplayAttributeData AttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Attributes")
	FGameplayAttributeData Defence;
};
