// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayEffectEC_Damage.generated.h"

/**
 * 招式的伤害效果。
 *	- 包含生命伤害和躯干伤害。
 *	- SetByCallerTags：
 *		SekiroLike.Data.HealthDamageMagnification	生命伤害倍率
 *		SekiroLike.Data.BalanceDamage				躯干伤害
 */
UCLASS(BlueprintType)
class SEKIROLIKE_API UGameplayEffectEC_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

protected:
	/** 根据角色属性值计算最终生命伤害
	 *  参数从左到右：技能倍率，攻击角色的攻击力，受击角色防御力，受击角色最大生命值 */
	virtual float GetFinalHealthDamage(float DamageMagnification, float SourceAttackDamage, float TargetDefence, float TargetMaxHealth = 0.0f) const;

public:
	UGameplayEffectEC_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
