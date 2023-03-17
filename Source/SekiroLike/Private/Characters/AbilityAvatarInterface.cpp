// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AbilityAvatarInterface.h"


// Add default functionality here for any IAbilityAvatarInterface functions that are not pure virtual.
void UAnimNotifyState_PostureType::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp)
	{
		auto AbilityAvatar = Cast<IAbilityAvatarInterface>(MeshComp->GetOwner());
		if (AbilityAvatar)
		{
			AbilityAvatar->SetPostureType(AnimPostureType);
		}
	}
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UAnimNotifyState_PostureType::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp)
	{
		auto AbilityAvatar = Cast<IAbilityAvatarInterface>(MeshComp->GetOwner());
		if (AbilityAvatar)
		{
			AbilityAvatar->SetPostureType(EPostureType::None);
		}
	}
	Super::NotifyEnd(MeshComp, Animation);
}
