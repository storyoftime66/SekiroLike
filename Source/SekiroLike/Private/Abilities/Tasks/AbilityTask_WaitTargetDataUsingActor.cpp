// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Tasks/AbilityTask_WaitTargetDataUsingActor.h"
#include "Abilities/GameplayAbilityTargetActor.h"

void UAbilityTask_WaitTargetDataUsingActor::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}
}

void UAbilityTask_WaitTargetDataUsingActor::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	Cancelled.Broadcast(Data);
	EndTask();
}

UAbilityTask_WaitTargetDataUsingActor* UAbilityTask_WaitTargetDataUsingActor::WaitTargetDataUsingActor(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	AGameplayAbilityTargetActor* InTargetActor)
{
	UAbilityTask_WaitTargetDataUsingActor* Task = NewAbilityTask<UAbilityTask_WaitTargetDataUsingActor>(OwningAbility, TaskInstanceName);
	Task->TargetActor = InTargetActor;
	return Task;
}

void UAbilityTask_WaitTargetDataUsingActor::Activate()
{
	if (TargetActor)
	{
		if (IsPendingKill())
		{
			return;
		}

		// 绑定委托
		ReadyDelegateHandle = TargetActor->TargetDataReadyDelegate.AddUObject(this, &UAbilityTask_WaitTargetDataUsingActor::OnTargetDataReadyCallback);
		CancelledDelegateHandle = TargetActor->CanceledDelegate.AddUObject(this, &UAbilityTask_WaitTargetDataUsingActor::OnTargetDataCancelledCallback);

		TargetActor->StartTargeting(Ability);
		
		SetWaitingOnAvatar();
	}
	else
	{
		EndTask();
	}
}

void UAbilityTask_WaitTargetDataUsingActor::OnDestroy(bool bInOwnerFinished)
{
	if (TargetActor)
	{
		TargetActor->ConfirmTargeting();

		// 移除委托
		TargetActor->TargetDataReadyDelegate.Remove(ReadyDelegateHandle);
		TargetActor->CanceledDelegate.Remove(CancelledDelegateHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}
