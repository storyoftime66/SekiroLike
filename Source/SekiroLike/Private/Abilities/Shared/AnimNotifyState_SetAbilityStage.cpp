// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Shared/AnimNotifyState_SetAbilityStage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/AbilityAvatarInterface.h"

void UAnimNotifyState_SetAbilityStage::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AbilityStageTag = SLAbilityTypes::GetAbilityStageTag(AbilityStage);

	if (AvatarActor == nullptr and MeshComp)
	{
		UObject* CurrentObject = MeshComp->GetOuter();
		AActor* Actor = nullptr;
		while (CurrentObject)
		{
			Actor = Cast<AActor>(CurrentObject);
			if (Actor)
			{
				AvatarActor = Actor;
				break;
			}
			CurrentObject = CurrentObject->GetOuter();
		}
	}

	if (const IAbilitySystemInterface* AbilityAvatar = Cast<IAbilitySystemInterface>(AvatarActor))
	{
		auto ASC = AbilityAvatar->GetAbilitySystemComponent();
		if (IsValid(ASC))
		{
			ASC->AddLooseGameplayTag(AbilityStageTag);
			if (bNotifyActorAtBeginning)
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AvatarActor, AbilityStageTag, FGameplayEventData());
			}
		}
	}

	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UAnimNotifyState_SetAbilityStage::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const IAbilitySystemInterface* AbilityAvatar = Cast<IAbilitySystemInterface>(AvatarActor))
	{
		const auto ASC = AbilityAvatar->GetAbilitySystemComponent();
		if (IsValid(ASC))
		{
			ASC->RemoveLooseGameplayTag(AbilityStageTag);
		}
	}
	
	Super::NotifyEnd(MeshComp, Animation);
}
