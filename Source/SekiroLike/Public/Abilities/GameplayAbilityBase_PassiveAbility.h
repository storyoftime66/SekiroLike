// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "GameplayAbilityBase_PassiveAbility.generated.h"

/**
 * 被动技能基类。
 *	- 被动技能包括受击硬直、受击倒地、被弹反。
 *	- 被动技能通常会打断大部分主动技能。
 *	- 通常由事件触发。
 */
UCLASS()
class SEKIROLIKE_API UGameplayAbilityBase_PassiveAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()
public:
	/** 在技能赋予时执行一次，通常用于持久型的技能。 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	bool bActivateOnce = false;
	
public:
	UGameplayAbilityBase_PassiveAbility();
};
