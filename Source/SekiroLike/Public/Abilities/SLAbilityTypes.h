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
	// Note: 使用静态成员会在初始化的时候获取未分配内存的 UGameplayTagsManager，导致崩溃
	// static TMap<EAbilityStage, FGameplayTag> AbilityStages;
	// static FGameplayTagContainer AbilityStageTags;
	// static FGameplayTagContainer ActiveStageTags;

	static FGameplayTag GetAbilityStageTag(const EAbilityStage StageEnum)
	{
		FGameplayTag ReturnTag;
		switch (StageEnum)
		{
		case EAbilityStage::Precast:
			ReturnTag = STAGE_PRECAST;
			break;
		case EAbilityStage::ActiveStart:
			ReturnTag = STAGE_ACTIVESTART;
			break;
		case EAbilityStage::ActiveEnd:
			ReturnTag = STAGE_ACTIVEEND;
			break;
		case EAbilityStage::Backswing:
			ReturnTag = STAGE_BACKSWING;
			break;
		default:
			ReturnTag = FGameplayTag::EmptyTag;
			break;
		}
		return ReturnTag;
	}

	static void ClearAllStageTags(const AActor* Avatar)
	{
		static FGameplayTagContainer AbilityStageTags;
		
		if (Avatar && Avatar->Implements<UAbilitySystemInterface>())
		{
			const auto ASC = Cast<IAbilitySystemInterface>(Avatar)->GetAbilitySystemComponent();
			if (ASC)
			{
				if (AbilityStageTags.Num() == 0)
				{
					AbilityStageTags.AddTag(STAGE_PRECAST);
					AbilityStageTags.AddTag(STAGE_ACTIVESTART);
					AbilityStageTags.AddTag(STAGE_ACTIVEEND);
					AbilityStageTags.AddTag(STAGE_BACKSWING);
				}
				ASC->RemoveLooseGameplayTags(AbilityStageTags);
			}
		}
	}

	static void ClearActiveStageTags(const AActor* Avatar)
	{
		static FGameplayTagContainer ActiveStageTags;
		
		if (Avatar && Avatar->Implements<UAbilitySystemInterface>())
		{
			const auto ASC = Cast<IAbilitySystemInterface>(Avatar)->GetAbilitySystemComponent();
			if (ASC)
			{
				if (ActiveStageTags.Num() == 0)
				{
					ActiveStageTags.AddTag(STAGE_ACTIVESTART);
					ActiveStageTags.AddTag(STAGE_ACTIVEEND);
				}
				ASC->RemoveLooseGameplayTags(ActiveStageTags);
			}
		}
	}
};
