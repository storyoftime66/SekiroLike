// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectEC_Damage.h"
#include "GameplayEffectEC_PlayerDamage.generated.h"

/**
 * 玩家角色招式的伤害效果。
 * 少了最大生命值伤害。
 */
UCLASS(BlueprintType)
class SEKIROLIKE_API UGameplayEffectEC_PlayerDamage : public UGameplayEffectEC_Damage
{
	GENERATED_BODY()

protected:
	virtual float GetFinalHealthDamage(float DamageMagnification, float SourceAttackDamage, float TargetDefence, float TargetMaxHealth) const override;
};
