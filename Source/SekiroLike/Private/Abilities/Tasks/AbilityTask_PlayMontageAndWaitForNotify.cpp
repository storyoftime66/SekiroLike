// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Tasks/AbilityTask_PlayMontageAndWaitForNotify.h"
#include "AbilitySystemComponent.h"


void UAbilityTask_PlayMontageAndWaitForNotify::Activate()
{
	check(AnimInstance);

	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;

	if (AbilitySystemComponent)
	{
		if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), Montage, 1.0f, NAME_None, 0.0f) > 0.f)
		{
			// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
			if (ShouldBroadcastAbilityTaskDelegates() == false)
			{
				return;
			}

			AbilityCancelledDelegate = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UAbilityTask_PlayMontageAndWaitForNotify::AbilityCancelled);

			BlendingOutDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWaitForNotify::MontageBlendingOut);
			AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, Montage);
			MontageEndedDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWaitForNotify::MontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);

			bPlayedMontage = true;
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWaitForNotify called on invalid AbilitySystemComponent"));
	}

	/** 播放失败，取消任务 */
	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWaitForNotify called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(Montage), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(NAME_None, FAnimNotifyEvent());
		}
	}

	// 绑定动画通知Handler
	// Note: 下面两行注释的代码是AnimInstance自带的方法，但调用后没有达到预期效果（不会触发）
	// AnimInstance->AddExternalNotifyHandler(this, FName("OnAnimNotifyBegin"));
	// AnimInstance->AddExternalNotifyHandler(this, FName("OnAnimNotifyEnd"));
	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UAbilityTask_PlayMontageAndWaitForNotify::NotifyBegin);
	AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UAbilityTask_PlayMontageAndWaitForNotify::NotifyEnd);

	SetWaitingOnAvatar();
}

void UAbilityTask_PlayMontageAndWaitForNotify::OnDestroy(bool bInOwnerFinished)
{
	// 取消绑定的回调
	if (IsValid(AnimInstance))
	{
		// AnimInstance->RemoveExternalNotifyHandler(this, FName("OnAnimNotifyBegin"));
		// AnimInstance->RemoveExternalNotifyHandler(this, FName("OnAnimNotifyEnd"));
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UAbilityTask_PlayMontageAndWaitForNotify::NotifyBegin);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UAbilityTask_PlayMontageAndWaitForNotify::NotifyEnd);
	}

	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(AbilityCancelledDelegate);
		if (bInOwnerFinished && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	Super::OnDestroy(bInOwnerFinished);
}

UAbilityTask_PlayMontageAndWaitForNotify* UAbilityTask_PlayMontageAndWaitForNotify::Create_PlayMontageAndWaitForNotifyTask(
	UGameplayAbility* OwingAbility,
	FName TaskInstanceName,
	UAnimMontage* MontageToPlay,
	UAnimInstance* InAnimInstance/* = nullptr */,
	FName InNotifyName/* = NAME_NONE */,
	bool StopWhenAbilityEnds/* = true */)
{
	UAbilityTask_PlayMontageAndWaitForNotify* Task = NewAbilityTask<UAbilityTask_PlayMontageAndWaitForNotify>(OwingAbility, TaskInstanceName);
	if (InAnimInstance)
	{
		Task->AnimInstance = InAnimInstance;
	}
	else
	{
		Task->AnimInstance = OwingAbility->GetCurrentActorInfo()->GetAnimInstance();
	}
	Task->Montage = MontageToPlay;
	Task->NotifyName = InNotifyName;
	Task->bStopWhenAbilityEnds = StopWhenAbilityEnds;

	return Task;
}


bool UAbilityTask_PlayMontageAndWaitForNotify::StopPlayingMontage()
{
	check(AnimInstance);

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (AbilitySystemComponent && Ability)
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability
			&& AbilitySystemComponent->GetCurrentMontage() == Montage)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(Montage);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}

void UAbilityTask_PlayMontageAndWaitForNotify::AbilityCancelled()
{
	if (StopPlayingMontage())
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(NAME_None, FAnimNotifyEvent());
		}
	}
	// 技能结束时会自动清理
}

void UAbilityTask_PlayMontageAndWaitForNotify::NotifyBegin(FName InNotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (InNotifyName.IsNone() || InNotifyName == NotifyName)
		{
			OnNotifyBegin.Broadcast(InNotifyName, *Payload.NotifyEvent);
		}
	}
}

void UAbilityTask_PlayMontageAndWaitForNotify::NotifyEnd(FName InNotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (InNotifyName.IsNone() || InNotifyName == NotifyName)
		{
			OnNotifyEnd.Broadcast(InNotifyName, *Payload.NotifyEvent);
		}
	}
}

void UAbilityTask_PlayMontageAndWaitForNotify::MontageEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(NAME_None, FAnimNotifyEvent());
		}
	}

	EndTask();
}

void UAbilityTask_PlayMontageAndWaitForNotify::MontageBlendingOut(UAnimMontage* InMontage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == Montage)
	{
		if (InMontage == Montage)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);
		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(NAME_None, FAnimNotifyEvent());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(NAME_None, FAnimNotifyEvent());
		}
	}
}
