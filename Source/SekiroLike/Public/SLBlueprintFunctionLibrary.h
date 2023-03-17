// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "SLBlueprintFunctionLibrary.generated.h"

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

};
