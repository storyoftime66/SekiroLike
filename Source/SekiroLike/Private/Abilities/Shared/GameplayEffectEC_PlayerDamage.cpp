// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Shared/GameplayEffectEC_PlayerDamage.h"

float UGameplayEffectEC_PlayerDamage::GetFinalHealthDamage(float DamageMagnification, float SourceAttackDamage, float TargetDefence, float TargetMaxHealth) const
{
	/** Note: 比原来的伤害计算公式少了10%的最大生命值伤害 */
	return (DamageMagnification * SourceAttackDamage) * 50.0f / (50.0f + TargetDefence);
}
