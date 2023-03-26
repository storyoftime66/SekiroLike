// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/SLAbilitySystemComponent.h"

#include "Abilities/GameplayAbilityBase_ActiveAbility.h"
#include "Characters/AbilityAvatarInterface.h"
#include "Characters/MovementAgentInterface.h"

void USLAbilitySystemComponent::ConsumeInputQueue()
{
	if (InputQueue.Num() != 0)
	{
		AbilityLocalInputPressed(InputQueue[0]);
		InputQueue.RemoveAt(0);
	}
}

void USLAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	// 技能激活失败时将技能加入队列
	if (auto ActiveAbility = Cast<UGameplayAbilityBase_ActiveAbility>(Ability))
	{
		if (ActiveAbility->InputId != INDEX_NONE)
		{
			if (InputQueue.Num() >= InputQueueMaxLength)
			{
				InputQueue.Empty();
			}
			InputQueue.Emplace(ActiveAbility->InputId);
		}
	}
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);
}

void USLAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	const auto AbilityAvatar = GetAvatarActor();

	// 指令队列逻辑
	if (InputQueue.Num() != 0)
	{
		if (auto MovementAgent = Cast<IMovementAgentInterface>(AbilityAvatar))
		{
			// 仅无移动输入时才自动释放队列中的技能
			if (MovementAgent->GetWorldMovementIntent().IsNearlyZero())
			{
				AbilityAvatar->GetWorldTimerManager().SetTimerForNextTick(this, &USLAbilitySystemComponent::ConsumeInputQueue);
			}
			else
			{
				InputQueue.Empty();
			}
		}
		else if (AbilityAvatar)
		{
			AbilityAvatar->GetWorldTimerManager().SetTimerForNextTick(this, &USLAbilitySystemComponent::ConsumeInputQueue);
		}
	}

	// 更新 SelfServiceQueryAbilityStates
	if (Ability != nullptr)
	{
		TSubclassOf<UGameplayAbility> AbilityClass = Ability->GetClass();
		uint8* Result = SelfServiceQueryAbilityStates.Find(AbilityClass);
		if (Result != nullptr)
		{
			if (bWasCancelled)
			{
				*Result |= 0b00000010;
			}
			else
			{
				*Result |= 0b00000001;
			}
		}
	}

	// Note: 必须将基类调用放在最后，否则蓝图调用的ClearInputQueue()无法清除队列
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
}

void USLAbilitySystemComponent::ClearInputQueue()
{
	InputQueue.Empty();
}

void USLAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// 绑定确认和取消的输入id
	if (ConfirmInputId >= 0)
	{
		GenericConfirmInputID = ConfirmInputId;
	}
	if (CancelInputId >= 0)
	{
		GenericCancelInputID = CancelInputId;
	}
}

FGameplayAbilitySpecHandle USLAbilitySystemComponent::GiveDisposableAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!IsValid(AbilityClass))
	{
		ABILITY_LOG(Warning, TEXT("GiveDisposableAbility called on invalid AbilityClass"));
		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpec Spec;
	TSubclassOf<UGameplayAbilityBase_ActiveAbility> ActiveAbilityClass = AbilityClass.Get();
	if (IsValid(ActiveAbilityClass))
	{
		auto Def = ActiveAbilityClass.GetDefaultObject();
		int32 InputId = INDEX_NONE;
		if (Def)
		{
			InputId = Def->InputId;
		}
		Spec = FGameplayAbilitySpec(ActiveAbilityClass, 1, InputId);
	}
	else
	{
		Spec = FGameplayAbilitySpec(AbilityClass);
	}

	// 下面代码修改自UAbilitySystemComponent::GiveAbilityAndActivateOnce()
	check(Spec.Ability);

	if (Spec.Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::NonInstanced || Spec.Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalOnly)
	{
		ABILITY_LOG(Error, TEXT("GiveDisposableAbility called on ability %s that is non instanced or won't execute on server, not allowed!"), *Spec.Ability->GetName());

		return FGameplayAbilitySpecHandle();
	}

	if (!IsOwnerActorAuthoritative())
	{
		ABILITY_LOG(Error, TEXT("GiveDisposableAbility called on ability %s on the client, not allowed!"), *Spec.Ability->GetName());

		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpecHandle AddedAbilityHandle = GiveAbility(Spec);
	FGameplayAbilitySpec* FoundSpec = FindAbilitySpecFromHandle(AddedAbilityHandle);

	if (FoundSpec)
	{
		FoundSpec->RemoveAfterActivation = true;
	}

	return AddedAbilityHandle;
}
