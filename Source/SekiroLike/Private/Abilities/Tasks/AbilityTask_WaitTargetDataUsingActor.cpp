// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Tasks/AbilityTask_WaitTargetDataUsingActor.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"

void UAbilityTask_WaitTargetDataUsingActor::RemoveTargetActorFromConfirmCancelInputs()
{
	// Note: 从TargetActor::EndPlay中抄的
	if (IsValid(TargetActor) && TargetActor->GenericDelegateBoundASC)
	{
		// We must remove ourselves from GenericLocalConfirmCallbacks/GenericLocalCancelCallbacks, since while these are bound they will inhibit any *other* abilities
		// that are bound to the same key.

		UAbilitySystemComponent* UnboundASC = nullptr;
		const FGameplayAbilityActorInfo* Info = (TargetActor->OwningAbility ? TargetActor->OwningAbility->GetCurrentActorInfo() : nullptr);
		if (Info && Info->IsLocallyControlled())
		{
			UAbilitySystemComponent* ASC = Info->AbilitySystemComponent.Get();
			if (ASC)
			{
				ASC->GenericLocalConfirmCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::ConfirmTargeting);
				ASC->GenericLocalCancelCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::CancelTargeting);

				UnboundASC = ASC;
			}
		}

		ensure(TargetActor->GenericDelegateBoundASC == UnboundASC); // Error checking that we have removed delegates from the same ASC we bound them to

		// 由于TargetActor 在EndPlay的时候还会去解绑一次，所以在这里解绑后要先置空
		TargetActor->GenericDelegateBoundASC = nullptr;
	}
}

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
	AGameplayAbilityTargetActor* InTargetActor,
	EGameplayTargetingConfirmation::Type InConfirmationType/* = EGameplayTargetingConfirmation::Instant */)
{
	UAbilityTask_WaitTargetDataUsingActor* Task = NewAbilityTask<UAbilityTask_WaitTargetDataUsingActor>(OwningAbility, TaskInstanceName);
	Task->TargetActor = InTargetActor;
	Task->ConfirmationType = InConfirmationType;
	return Task;
}

void UAbilityTask_WaitTargetDataUsingActor::Activate()
{
	if (TargetActor)
	{
		if (IsValid(this))
		{
			return;
		}

		// 绑定委托
		ReadyDelegateHandle = TargetActor->TargetDataReadyDelegate.AddUObject(this, &UAbilityTask_WaitTargetDataUsingActor::OnTargetDataReadyCallback);
		CancelledDelegateHandle = TargetActor->CanceledDelegate.AddUObject(this, &UAbilityTask_WaitTargetDataUsingActor::OnTargetDataCancelledCallback);

		TargetActor->StartTargeting(Ability);

		if (TargetActor->ShouldProduceTargetData())
		{
			if (ConfirmationType == EGameplayTargetingConfirmation::Instant)
			{
				TargetActor->ConfirmTargeting();
			}
			else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
			{
				TargetActor->BindToConfirmCancelInputs();
			}
		}

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
		if (ConfirmationType == EGameplayTargetingConfirmation::Custom)
		{
			TargetActor->ConfirmTargeting();
		}

		// 移除任务委托
		TargetActor->TargetDataReadyDelegate.Remove(ReadyDelegateHandle);
		TargetActor->CanceledDelegate.Remove(CancelledDelegateHandle);

		if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			RemoveTargetActorFromConfirmCancelInputs();
		}
	}

	Super::OnDestroy(bInOwnerFinished);
}
