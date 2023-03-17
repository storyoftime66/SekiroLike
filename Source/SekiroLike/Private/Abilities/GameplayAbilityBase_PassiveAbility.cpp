// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbilityBase_PassiveAbility.h"

UGameplayAbilityBase_PassiveAbility::UGameplayAbilityBase_PassiveAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("SekiroLike.Ability.PassiveAbility"));
}
