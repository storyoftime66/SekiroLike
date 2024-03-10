// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBlueprintFunctionLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
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
		UE_LOG(LogTemp, Warning, TEXT("USLBlueprintFunctionLibrary::GetTagSetByCallerMagnitude called with invalid SpecHandle"));
	}

	return 0.0f;
}

void USLBlueprintFunctionLibrary::GetEffectSpecAssetTagsFromHandle(FGameplayEffectSpecHandle SpecHandle, FGameplayTagContainer& OutTags)
{
	OutTags.Reset();
	if (SpecHandle.IsValid())
	{
		const auto Spec = SpecHandle.Data.Get();
		if (Spec != nullptr)
		{
			Spec->GetAllAssetTags(OutTags);
		}
	}
}

void USLBlueprintFunctionLibrary::GetEffectSpecDynamicAssetTagsFromHandle(FGameplayEffectSpecHandle SpecHandle, FGameplayTagContainer& OutTags)
{
	OutTags.Reset();
	if (SpecHandle.IsValid())
	{
		const auto Spec = SpecHandle.Data.Get();
		if (Spec != nullptr)
		{
			OutTags.AppendTags(Spec->DynamicAssetTags);
		}
	}
}

FGameplayAbilitySpecHandle USLBlueprintFunctionLibrary::GiveAbilityAndActivateOnce(UAbilitySystemComponent* InASC, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayEventData Payload)
{
	if (IsValid(InASC) && IsValid(AbilityClass))
	{
		auto Spec = FGameplayAbilitySpec(AbilityClass);
		return InASC->GiveAbilityAndActivateOnce(Spec, &Payload);
	}

	return FGameplayAbilitySpecHandle();
}

void USLBlueprintFunctionLibrary::RemoveAssetTag(FGameplayEffectSpecHandle SpecHandle, FGameplayTag GameplayTag)
{
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (Spec)
	{
		Spec->DynamicAssetTags.RemoveTag(GameplayTag);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UAbilitySystemBlueprintLibrary::RemoveAssetTag called with invalid SpecHandle"));
	}
}

void USLBlueprintFunctionLibrary::LogEffectSpecHandle(FGameplayEffectSpecHandle SpecHandle)
{
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (Spec)
	{
		UE_LOG(LogTemp, Display, TEXT("====================================="));
		UE_LOG(LogTemp, Display, TEXT("[Spec id] %d"), Spec);
		UE_LOG(LogTemp, Display, TEXT("[DynamicAssetTags Num] %d"), Spec->DynamicAssetTags.Num());
		UE_LOG(LogTemp, Display, TEXT("[Effect Def id] %d"), Spec->Def);
		if (Spec->Def)
		{
			// UE_LOG(LogTemp, Display, TEXT("[Effect Def id] %d"), Spec->Def->Dynamic);
		}
	}
}

void USLBlueprintFunctionLibrary::LogActiveAbilities(AActor* Actor)
{
	if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor))
	{
		UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
		if (IsValid(ASC))
		{
			UE_LOG(LogTemp, Display, TEXT("====== LogActiveAbilities Begin ======"));
			for (auto Spec : ASC->GetActivatableAbilities())
			{
				if (Spec.IsActive())
				{
					UE_LOG(LogTemp, Display, TEXT("ActiveAbilityName: %s"), *GetNameSafe(Spec.Ability));
				}
			}
			UE_LOG(LogTemp, Display, TEXT("====== LogActiveAbilities End ======"));
		}
	}
}

bool USLBlueprintFunctionLibrary::IsEqual(FGameplayEffectSpecHandle SpecHandle1, FGameplayEffectSpecHandle SpecHandle2)
{
	return SpecHandle1 == SpecHandle2;
}
