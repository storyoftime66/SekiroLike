// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Tasks/AbilityTask_WaitForNotify.h"

void UAbilityTask_WaitForNotify::Activate()
{
	check(AnimInstance);

	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UAbilityTask_WaitForNotify::NotifyBegin);
	AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UAbilityTask_WaitForNotify::NotifyEnd);

	if (Ability)
	{
		AbilityCancelledDelegate = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UAbilityTask_WaitForNotify::OnAbilityCancelled);
	}

	SetWaitingOnAvatar();
}

void UAbilityTask_WaitForNotify::OnDestroy(bool bInOwnerFinished)
{
	if (IsValid(AnimInstance))
	{
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UAbilityTask_WaitForNotify::NotifyBegin);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UAbilityTask_WaitForNotify::NotifyEnd);
	}

	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(AbilityCancelledDelegate);
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitForNotify::OnAbilityCancelled()
{
	EndTask();
}

void UAbilityTask_WaitForNotify::NotifyBegin(FName InNotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (NotifyName.IsNone() || InNotifyName == NotifyName)
		{
			OnNotifyBegin.Broadcast(InNotifyName, *Payload.NotifyEvent);
		}
	}
}

void UAbilityTask_WaitForNotify::NotifyEnd(FName InNotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (NotifyName.IsNone() || InNotifyName == NotifyName)
		{
			OnNotifyEnd.Broadcast(InNotifyName, *Payload.NotifyEvent);
		}
	}
}

UAbilityTask_WaitForNotify* UAbilityTask_WaitForNotify::Create_WaitForNotify(
	UGameplayAbility* OwingAbility,
	FName TaskInstanceName,
	FName InSpecificNotifyName,
	UAnimInstance* AnimInstance)
{
	UAbilityTask_WaitForNotify* Task = NewAbilityTask<UAbilityTask_WaitForNotify>(OwingAbility, TaskInstanceName);
	if (AnimInstance)
	{
		Task->AnimInstance = AnimInstance;
	}
	else
	{
		Task->AnimInstance = OwingAbility->GetCurrentActorInfo()->GetAnimInstance();
	}
	Task->NotifyName = InSpecificNotifyName;

	return Task;
}
