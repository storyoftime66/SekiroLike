// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "SLBlueprintFunctionLibrary.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

/**
 * 包含一些通用的函数，包括：
 * - AbilitySystemBlueprintLibrary的一点补充
 */
UCLASS(meta=(ScriptName="SekiroLikeLibrary"))
class SEKIROLIKE_API USLBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Gets a gameplay tag Set By Caller magnitude value */
	UFUNCTION(BlueprintPure, Category = "Ability|GameplayEffect")
	static float GetTagSetByCallerMagnitude(FGameplayEffectSpecHandle SpecHandle, FGameplayTag DataTag);

	/** Gets a gameplay tag Set By Caller magnitude value */
	UFUNCTION(BlueprintPure, Category = "Ability|GameplayEffect")
	static void GetEffectSpecAssetTagsFromHandle(FGameplayEffectSpecHandle SpecHandle, FGameplayTagContainer& OutTags);

	/** Gets a gameplay tag Set By Caller magnitude value */
	UFUNCTION(BlueprintPure, Category = "Ability|GameplayEffect")
	static void GetEffectSpecDynamicAssetTagsFromHandle(FGameplayEffectSpecHandle SpecHandle, FGameplayTagContainer& OutTags);

	/** 赋予一个技能并激活一次，然后移除。通常用于执行工具技能。 */
	UFUNCTION(BlueprintCallable, Category="Ability")
	static FGameplayAbilitySpecHandle GiveAbilityAndActivateOnce(UAbilitySystemComponent* InASC, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayEventData Payload);

	/** Removes GameplayTag from this instance of the effect */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static void RemoveAssetTag(FGameplayEffectSpecHandle SpecHandle, FGameplayTag GameplayTag);

	/** 在日志中打印EffectSpecHandle的相关信息 */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static void LogEffectSpecHandle(FGameplayEffectSpecHandle SpecHandle);

	/** 在日志中打印未结束的技能 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void LogActiveAbilities(AActor* Actor);

	/** 两个SpecHandle是否相等 */
	UFUNCTION(BlueprintCallable, Category="Ability|GameplayEffect")
	static bool IsEqual(FGameplayEffectSpecHandle SpecHandle1, FGameplayEffectSpecHandle SpecHandle2);
};
