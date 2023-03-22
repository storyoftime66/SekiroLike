// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SLAbilitySystemComponent.generated.h"


/**
 * 稍微修改功能的技能组件。
 * TODO: 指令队列，但还没有确定好具体需求
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SEKIROLIKE_API USLAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:
	/////////////////////////////////////////////
	/// 输入相关
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
	/// 指令队列
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
