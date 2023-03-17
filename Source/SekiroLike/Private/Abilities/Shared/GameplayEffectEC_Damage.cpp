// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Shared/GameplayEffectEC_Damage.h"

#include "Characters/CharAttributeSet.h"

/** 存储属性Capture信息的结构，参考自GASDocumentation */
struct SLDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);

	SLDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharAttributeSet, AttackDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharAttributeSet, Defence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharAttributeSet, MaxHealth, Target, false);
	}
};

static const SLDamageStatics& DamageStatics()
{
	static SLDamageStatics DamageStatics;
	return DamageStatics;
}

float UGameplayEffectEC_Damage::GetFinalHealthDamage(float DamageMagnification, float SourceAttackDamage, float TargetDefence, float TargetMaxHealth) const
{
	return (DamageMagnification * SourceAttackDamage + TargetMaxHealth * 0.1f) * 50.0f / (50.0f + TargetDefence);
}

UGameplayEffectEC_Damage::UGameplayEffectEC_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenceDef);
	RelevantAttributesToCapture.Add(DamageStatics().MaxHealthDef);
}

void UGameplayEffectEC_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	auto SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	auto TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Spec.GetAllAssetTags(SpecAssetTags);

	// UE_LOG(LogTemp, Display, TEXT("[SourceTags] %s"), *SourceTags->ToString());
	// UE_LOG(LogTemp, Display, TEXT("[TargetTags] %s"), *TargetTags->ToString());

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// 有以下tag表示该次攻击被格挡或被弹反
	static const FGameplayTag DamageBlockedTag = FGameplayTag::RequestGameplayTag("SekiroLike.Effect.DynamicModification.DamageBlocked");
	static const FGameplayTag DamageParriedTag = FGameplayTag::RequestGameplayTag("SekiroLike.Effect.DynamicModification.DamageParried");
	FGameplayTagContainer DamageBlockedTagContainer;
	DamageBlockedTagContainer.AddTag(DamageBlockedTag);
	DamageBlockedTagContainer.AddTag(DamageParriedTag);

	// 计算应受的生命伤害
	float FinalHealthDamage;
	if (SpecAssetTags.HasAny(DamageBlockedTagContainer))
	{
		FinalHealthDamage = 0.0f;
	}
	else
	{
		float TargetDefence = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenceDef, EvaluateParameters, TargetDefence);
		TargetDefence = FMath::Max(TargetDefence, 0.0f);

		float SourceAttackDamage = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDamageDef, EvaluateParameters, SourceAttackDamage);
		SourceAttackDamage = FMath::Max(SourceAttackDamage, 0.0f);

		float TargetMaxHealth = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MaxHealthDef, EvaluateParameters, TargetMaxHealth);
		TargetMaxHealth = FMath::Max(TargetMaxHealth, 0.0f);

		// 生命伤害倍率
		static const FGameplayTag HealthDamageMagnificationDataTag = FGameplayTag::RequestGameplayTag("SekiroLike.Data.HealthDamageMagnification");
		float HealthDamageMagnification = Spec.GetSetByCallerMagnitude(HealthDamageMagnificationDataTag, true, 1.0f);
		HealthDamageMagnification = FMath::Max(HealthDamageMagnification, 0.0f);

		// 生命伤害计算公式
		FinalHealthDamage = GetFinalHealthDamage(HealthDamageMagnification, SourceAttackDamage, TargetDefence, TargetMaxHealth);
	}

	if (FinalHealthDamage > 0.0f)
	{
		FinalHealthDamage *= -1.0f;
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, FinalHealthDamage));
	}

	// 计算应受的躯干伤害
	static const FGameplayTag BalanceDamageDataTag = FGameplayTag::RequestGameplayTag("SekiroLike.Data.BalanceDamage");
	float BalanceDamage = Spec.GetSetByCallerMagnitude(BalanceDamageDataTag, true, 20.0f);
	BalanceDamage = FMath::Max(BalanceDamage, 0.0f);

	// 格挡和弹反会削减受到的躯干伤害
	if (SpecAssetTags.HasTag(DamageParriedTag))
	{
		BalanceDamage *= 0.4f;
	}
	else if (SpecAssetTags.HasTag(DamageBlockedTag))
	{
		BalanceDamage *= 0.8f;
	}

	if (BalanceDamage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharAttributeSet::GetBalanceAttribute(), EGameplayModOp::Additive, BalanceDamage));
	}
}
