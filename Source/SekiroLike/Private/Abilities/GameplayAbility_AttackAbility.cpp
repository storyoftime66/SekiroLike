// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_AttackAbility.h"

#include "AbilitySystemComponent.h"


void UGameplayAbility_AttackAbility::MakeEventDataFromTargetData_Implementation(FGameplayAbilityTargetDataHandle TargetDataHandle, AActor*& OutActor, FGameplayEventData& OutEventData)
{
	// 重置输出引用
	OutActor = nullptr;
	OutEventData = FGameplayEventData();

	const auto ASC = GetAbilitySystemComponentFromActorInfo_Checked();
	auto TempEffectSpecHandle = ASC->MakeEffectContext();
	
	// 找一个可用的HitResult
	for (int i = 0; i < TargetDataHandle.Num(); ++i)
	{
		if (TargetDataHandle.IsValid(i))
		{
			const FGameplayAbilityTargetData* TargetData = TargetDataHandle.Get(0);
			if (TargetData->HasHitResult() && TargetData->GetHitResult() && TargetData->GetHitResult()->GetActor())
			{
				TempEffectSpecHandle.AddHitResult(*TargetData->GetHitResult());
				OutActor = TargetData->GetHitResult()->GetActor();
				break;
			}
		}
	}

	// 拼装EventData
	OutEventData.Instigator = GetAvatarActorFromActorInfo();
	OutEventData.Target = OutActor;
	OutEventData.ContextHandle = TempEffectSpecHandle;
	OutEventData.TargetData = TargetDataHandle;
}

UObject* UGameplayAbility_AttackAbility::MakeOutgoingEffectSpecObject_Implementation(TSubclassOf<UGameplayEffect> EffectClass)
{
	return nullptr;
}
