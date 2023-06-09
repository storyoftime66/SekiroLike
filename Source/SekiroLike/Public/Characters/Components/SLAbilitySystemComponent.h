// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SLAbilitySystemComponent.generated.h"

#define ABILITY_END_MASK 0b00000001
#define ABILITY_CANCEL_MASK 0b00000010

/**
 * 稍微修改功能的技能组件，方便玩家和AI使用。
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SEKIROLIKE_API USLAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	/////////////////////////////////////////////
	/// AI相关
	/////////////////////////////////////////////

	/**
	 * 自助查询的技能状态，可从外部查询某个特定技能是否已结束或已取消。
	 * 用于AI行为树更新状态。
	 */
	TMap<TSubclassOf<UGameplayAbility>, uint8> SelfServiceQueryAbilityStates;

public:
	/** 技能是否已经结束 */
	bool IsAbilityEnded(TSubclassOf<UGameplayAbility> AbilityClass) const;

	/** 技能是否已取消 */
	bool IsAbilityCancelled(TSubclassOf<UGameplayAbility> AbilityClass) const;

	/** 添加到查询列表 */
	void AddSelfServiceQueryAbility(TSubclassOf<UGameplayAbility> AbilityClass);

	/** 从查询列表移除 */
	void RemoveSelfServiceQueryAbility(TSubclassOf<UGameplayAbility> AbilityClass) { SelfServiceQueryAbilityStates.Remove(AbilityClass); }

protected:
	/////////////////////////////////////////////
	/// 玩家输入相关
	/////////////////////////////////////////////
	/** 确认选择目标的输入ID
	 *  - 依赖ActiveAbility，需要ASC被赋予了与该ID有相同ID的ActiveAbility才可触发
	 *  - @see also UGameplayAbilityBase_ActiveAbility 的 GiveAbilityTo 方法 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Components")
	int32 ConfirmInputId = INDEX_NONE;
	/** 取消选择目标的输入ID
	 *  - 依赖ActiveAbility，需要ASC被赋予了与该ID有相同ID的ActiveAbility才可触发
	 *  - @see also UGameplayAbilityBase_ActiveAbility 的 GiveAbilityTo 方法 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Components")
	int32 CancelInputId = INDEX_NONE;

protected:
	/////////////////////////////////////////////
	/// 玩家指令队列
	///	- 暂时还不知道长度大于1的指令队列有什么用
	/////////////////////////////////////////////

	/** 输入指令队列, 记录InputID */
	UPROPERTY()
	TArray<int32> InputQueue;

	/** 指令队列长度 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	int32 InputQueueMaxLength = 1;

	/** 执行指令队列中的技能 */
	UFUNCTION()
	void ConsumeInputQueue();

public:
	/** 清空指令队列*/
	UFUNCTION(BlueprintCallable, Category="SekiroLike|Ability")
	void ClearInputQueue();

protected:
	virtual void BeginPlay() override;

public:
	// 被Block时会激活失败
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;

	/** 赋予一次性的技能，激活一次后会自动移除，但不会立刻激活。 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|Ability")
	FGameplayAbilitySpecHandle GiveDisposableAbility(TSubclassOf<UGameplayAbility> AbilityClass);
};
