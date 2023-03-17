// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_MontageCombo.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitForNotify.h"
#include "Characters/AbilityAvatarInterface.h"
#include "Kismet/GameplayStatics.h"

void UGameplayAbility_MontageCombo::OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer&)
{
	if (this == Ability and Ability->IsActive())
	{
		bAutoCombo = true;
	}
}

void UGameplayAbility_MontageCombo::ActivateForNextTick_Implementation()
{
	const auto ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->TryActivateAbilityByClass(GetClass());
	}
}

UGameplayAbility_MontageCombo::UGameplayAbility_MontageCombo()
{
}

void UGameplayAbility_MontageCombo::ResetCombo()
{
	CurrentCombo = 0;
	bAutoCombo = false;
}

void UGameplayAbility_MontageCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo) or ComboMontages.Num() == 0)
	{
		return;
	}

	// 移除技能阶段Tag
	AActor* Avatar = ActorInfo->AvatarActor.Get();
	if (Avatar and Avatar->Implements<UAbilityAvatarInterface>())
	{
		Cast<IAbilityAvatarInterface>(Avatar)->ClearAbilityStage();
	}

	// 先执行蓝图的逻辑
	if (bHasBlueprintActivate)
	{
		K2_ActivateAbility();
	}
	else if (bHasBlueprintActivateFromEvent)
	{
		K2_ActivateAbilityFromEvent(*TriggerEventData);
	}

	const float NowTime = UGameplayStatics::GetTimeSeconds(this);
	if (NowTime - LastTimeActivated > MaxComboInterval)
	{
		ResetCombo();
	}
	LastTimeActivated = NowTime;

	// 使用任务播放动画
	const FName TaskName("PlayMontage_In_MontageCombo");
	UAnimMontage* MontageToPlay = ComboMontages[CurrentCombo];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, MontageToPlay, 1.0f, NAME_None, false);
	// 结束时自动退出
	MontageTask->OnCompleted.AddDynamic(this, &UGameplayAbility_MontageCombo::EndAbilityActively);
	// 技能执行失败
	MontageTask->OnCancelled.AddDynamic(this, &UGameplayAbility_MontageCombo::EndAbilityActively);

	MontageTask->ReadyForActivation();
}

void UGameplayAbility_MontageCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bWasCancelled)
	{
		ResetCombo();
	}
	else
	{
		CurrentCombo = (CurrentCombo + 1) % ComboMontages.Num();

		// 多次循环间不自动连击
		if (CurrentCombo == 0)
		{
			bAutoCombo = false;
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// 触发自动连击
	if (bAutoCombo and ActorInfo)
	{
		bAutoCombo = false;
		const auto Avatar = ActorInfo->AvatarActor.Get();
		if (Avatar)
		{
			Avatar->GetWorldTimerManager().SetTimerForNextTick(this, &UGameplayAbility_MontageCombo::ActivateForNextTick);
		}
	}
}

void UGameplayAbility_MontageCombo::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	const float NowTime = UGameplayStatics::GetTimeSeconds(this);
	LastTimePressed = NowTime;

	// Note: 避免连续快速触发技能
	if (IsActive() or NowTime - LastTimeActivated < 0.3f)
	{
		bAutoCombo = true;
	}
}

void UGameplayAbility_MontageCombo::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	LastTimeReleased = UGameplayStatics::GetTimeSeconds(this);
}
