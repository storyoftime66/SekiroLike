// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Shared/AnimNotify_SetAbilityStage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/AbilityAvatarInterface.h"

void UAnimNotify_SetAbilityStage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	const FGameplayTag AbilityStageTag = SLAbilityTypes::GetAbilityStageTag(AbilityStage);

	if (MeshComp)
	{
		UObject* CurrentObject = MeshComp->GetOuter();
		AActor* Actor = nullptr;
		while (CurrentObject)
		{
			Actor = Cast<AActor>(CurrentObject);
			if (Actor)
			{
				break;
			}
			CurrentObject = CurrentObject->GetOuter();
		}

		if (Actor and Actor->Implements<UAbilityAvatarInterface>())
		{
			IAbilityAvatarInterface* AbilityAvatar = CastChecked<IAbilityAvatarInterface>(Actor);
			AbilityAvatar->ClearAbilityStage();
			if (AbilityStageTag.IsValid())
			{
				AbilityAvatar->AddAbilityStage(AbilityStageTag);
				if (bNotifyActor)
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, AbilityStageTag, FGameplayEventData());
				}
			}
		}
	}

	Super::Notify(MeshComp, Animation);
}
