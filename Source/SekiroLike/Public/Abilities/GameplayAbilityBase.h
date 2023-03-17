// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilityBase.generated.h"

UCLASS(Abstract)
class SEKIROLIKE_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	/** 获取技能要播放的Montage */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="SekiroLike|Ability")
	UAnimMontage* GetAbilityMontage();
};
