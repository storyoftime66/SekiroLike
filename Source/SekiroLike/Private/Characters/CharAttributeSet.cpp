// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharAttributeSet.h"

UCharAttributeSet::UCharAttributeSet():
	MaxHealth(100.0f),
	Health(100.0f),
	MaxBalance(100.0f),
	Balance(0.0f),
	AttackDamage(20.0f),
	Defence(10.0f)
{
}

void UCharAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// 不让血量和躯干值溢出
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	if (Attribute == GetBalanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxBalance());
	}
}
