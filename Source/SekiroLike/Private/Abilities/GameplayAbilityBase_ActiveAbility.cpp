// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbilityBase_ActiveAbility.h"
#include "Abilities/SLAbilityTypes.h"

#include "AbilitySystemComponent.h"

FGameplayTag UGameplayAbilityBase_ActiveAbility::ABILITY_TAG = FGameplayTag::RequestGameplayTag("SekiroLike.Ability.ActiveAbility");

AGameplayAbilityTargetActor* UGameplayAbilityBase_ActiveAbility::GetTargetActor_Implementation()
{
	return TargetActor;
}

UGameplayAbilityBase_ActiveAbility::UGameplayAbilityBase_ActiveAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 多个主动技能不能同时释放
	AbilityTags.AddTag(ABILITY_TAG);
	// 使用一个主动技能会取消其他主动技能（但在Active阶段会使用失败）
	CancelAbilitiesWithTag.AddTag(ABILITY_TAG);

	ActivationBlockedTags.AddTag(SLAbilityTypes::GetAbilityStageTag(EAbilityStage::ActiveStart));
	ActivationBlockedTags.AddTag(SLAbilityTypes::GetAbilityStageTag(EAbilityStage::ActiveEnd));
}

void UGameplayAbilityBase_ActiveAbility::GiveAbilityTo(TSubclassOf<UGameplayAbilityBase_ActiveAbility> AbilityClass, UAbilitySystemComponent* ASC, UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!IsValid(AbilityClass) or !IsValid(ASC))
	{
		return;
	}

	// 赋予技能
	const int32 AbilityLevel = 1;
	auto AbilityDef = AbilityClass.GetDefaultObject();
	check(AbilityDef);
	const int32 AbilityInputID = AbilityDef->InputId;
	auto SpecHandle = ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, AbilityLevel, AbilityInputID));

	if (!IsValid(EnhancedInputComponent) or !AbilityDef->bUseActionBinding)
	{
		return;
	}

	auto IA_Ability = AbilityDef->InputAction;
	auto InputID = AbilityDef->InputId;
	if (IA_Ability == nullptr or IA_Ability->ValueType != EInputActionValueType::Boolean or AbilityDef->InputId == INDEX_NONE)
	{
		return;
	}

	// 绑定技能释放到输入
	auto TriggerEvent = AbilityDef->TriggerEvent;
	if (TriggerEvent == ETriggerEvent::Started or TriggerEvent == ETriggerEvent::Triggered)
	{
		EnhancedInputComponent->BindAction(IA_Ability, TriggerEvent, ASC, &UAbilitySystemComponent::AbilityLocalInputPressed, InputID);
	}
	EnhancedInputComponent->BindAction(IA_Ability, ETriggerEvent::Completed, ASC, &UAbilitySystemComponent::AbilityLocalInputReleased, InputID);
}

void UGameplayAbilityBase_ActiveAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo)
	{
		SLAbilityTypes::ClearActiveStageTags(ActorInfo->AvatarActor.Get());
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbilityBase_ActiveAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (ActorInfo)
	{
		auto ASC = ActorInfo->AbilitySystemComponent.Get();
		// TODO: 取消输入中的绑定
	}
}
