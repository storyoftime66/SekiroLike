// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SLAbilityTypes.h"

TMap<EAbilityStage, FGameplayTag> SLAbilityTypes::AbilityStages = InitializeAbilityStages();

const FGameplayTagContainer SLAbilityTypes::AbilityStageTags = FGameplayTagContainer::CreateFromArray(InitializeAbilityStageTags());
const FGameplayTagContainer SLAbilityTypes::ActiveStageTags = FGameplayTagContainer::CreateFromArray(InitializeActiveStageTags());

TMap<EAbilityStage, FGameplayTag> SLAbilityTypes::InitializeAbilityStages()
{
	TMap<EAbilityStage, FGameplayTag> Temp;
	Temp.Add(EAbilityStage::Precast, STAGE_PRECAST);
	Temp.Add(EAbilityStage::ActiveStart, STAGE_ACTIVESTART);
	Temp.Add(EAbilityStage::ActiveEnd, STAGE_ACTIVEEND);
	Temp.Add(EAbilityStage::Backswing, STAGE_BACKSWING);
	return Temp;
}

TArray<FGameplayTag> SLAbilityTypes::InitializeAbilityStageTags()
{
	TArray<FGameplayTag> TagArray;
	AbilityStages.GenerateValueArray(TagArray);
	return TagArray;
}

TArray<FGameplayTag> SLAbilityTypes::InitializeActiveStageTags()
{
	TArray<FGameplayTag> TagArray;
	TagArray.Add(STAGE_ACTIVESTART);
	TagArray.Add(STAGE_ACTIVEEND);
	return TagArray;
}
