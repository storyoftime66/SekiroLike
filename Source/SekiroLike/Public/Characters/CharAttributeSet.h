// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CharAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/**
 * 角色属性集。
 * 包括血量、躯干值、攻击力、防御力。
 */
UCLASS()
class SEKIROLIKE_API UCharAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCharAttributeSet();

	/** 血量上限 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCharAttributeSet, MaxHealth)

	/** 血量 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCharAttributeSet, Health)

	/** 躯干值上限 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Attributes")
	FGameplayAttributeData MaxBalance;
	ATTRIBUTE_ACCESSORS(UCharAttributeSet, MaxBalance)

	/** 躯干值 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Attributes")
	FGameplayAttributeData Balance;
	ATTRIBUTE_ACCESSORS(UCharAttributeSet, Balance)

	/** 攻击伤害 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Attributes")
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UCharAttributeSet, AttackDamage)

	/** 防御力 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Attributes")
	FGameplayAttributeData Defence;
	ATTRIBUTE_ACCESSORS(UCharAttributeSet, Defence)

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
};
