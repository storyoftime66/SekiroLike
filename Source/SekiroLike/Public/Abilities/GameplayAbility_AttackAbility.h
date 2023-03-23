// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase_ActiveAbility.h"
#include "GameplayAbility_AttackAbility.generated.h"

/**
 * 攻击技能
 * - 添加了一些攻击技能常用的方法。
 */
UCLASS()
class SEKIROLIKE_API UGameplayAbility_AttackAbility : public UGameplayAbilityBase_ActiveAbility
{
	GENERATED_BODY()

protected:
	/** 使用TargetData制作EventData
	 *  - 默认每个TargetDataHandle仅包含一个Actor
	 *  - 默认只填充了Instigator、Target、EffectContext、TargetData，其余需要自己拼装 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="SekiroLike|Attack Ability")
	void MakeEventDataFromTargetData(FGameplayAbilityTargetDataHandle TargetDataHandle, AActor*& OutActor, FGameplayEventData& OutEventData);

	/** 生成一个EffectSpecObject，用于在技能之间传递Effect */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="SekiroLike|Attack Ability")
	UObject* MakeOutgoingEffectSpecObject(TSubclassOf<UGameplayEffect> EffectClass);
};
