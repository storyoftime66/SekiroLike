// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"

#define STAGE_PRECAST		FGameplayTag::RequestGameplayTag("SekiroLike.AbilityStage.Precast")
#define STAGE_ACTIVESTART	FGameplayTag::RequestGameplayTag("SekiroLike.AbilityStage.ActiveStart")
#define STAGE_ACTIVEEND		FGameplayTag::RequestGameplayTag("SekiroLike.AbilityStage.ActiveEnd")
#define STAGE_BACKSWING		FGameplayTag::RequestGameplayTag("SekiroLike.AbilityStage.Backswing")


struct SLAbilityTypes;

/** 技能阶段，每个阶段对应一个Gameplay标签 */
UENUM(BlueprintType)
enum class EAbilityStage: uint8
{
	None = 0,
	Precast,
	ActiveStart,
	ActiveEnd,
	Backswing,
	Max
};

struct SLAbilityTypes
{
	static TMap<EAbilityStage, FGameplayTag> AbilityStages;
	static const FGameplayTagContainer AbilityStageTags;
	static const FGameplayTagContainer ActiveStageTags;

	static TMap<EAbilityStage, FGameplayTag> InitializeAbilityStages();
	static TArray<FGameplayTag> InitializeAbilityStageTags();
	static TArray<FGameplayTag> InitializeActiveStageTags();

	static FGameplayTag GetAbilityStageTag(EAbilityStage StageEnum)
	{
		if (StageEnum != EAbilityStage::None and StageEnum < EAbilityStage::Max)
		{
			return AbilityStages[StageEnum];
		}
		return FGameplayTag::EmptyTag;

		// switch (StageEnum)
		// {
		// case EAbilityStage::Precast:
		// 	return STAGE_PRECAST;
		// case EAbilityStage::ActiveStart:
		// 	return STAGE_ACTIVESTART;
		// case EAbilityStage::ActiveEnd:
		// 	return STAGE_ACTIVEEND;
		// case EAbilityStage::Backswing:
		// 	return STAGE_BACKSWING;
		// default:
		// 	return FGameplayTag::EmptyTag;
		// }
	}

	static void ClearAllStageTags(const AActor* Avatar)
	{
		if (Avatar and Avatar->Implements<UAbilitySystemInterface>())
		{
			const auto ASC = Cast<IAbilitySystemInterface>(Avatar)->GetAbilitySystemComponent();
			if (ASC)
			{
				ASC->RemoveLooseGameplayTags(AbilityStageTags);
			}
		}
	}

	static void ClearActiveStageTags(const AActor* Avatar)
	{
		if (Avatar and Avatar->Implements<UAbilitySystemInterface>())
		{
			const auto ASC = Cast<IAbilitySystemInterface>(Avatar)->GetAbilitySystemComponent();
			if (ASC)
			{
				ASC->RemoveLooseGameplayTags(ActiveStageTags);
			}
		}
	}
};
