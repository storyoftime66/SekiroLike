// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBlueprintFunctionLibrary.h"

#include "GameplayEffect.h"

float USLBlueprintFunctionLibrary::GetTagSetByCallerMagnitude(FGameplayEffectSpecHandle SpecHandle, FGameplayTag DataTag)
{
	const FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (Spec)
	{
		return Spec->GetSetByCallerMagnitude(DataTag, true, 0.0f);
		
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("USLBlueprintFunctionLibrary::GetTagSetByCallerMagnitude called with invalid SpecHandle"));
	}

	return 0.0f;
}
