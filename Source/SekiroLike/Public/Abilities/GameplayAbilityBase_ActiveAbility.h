// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayAbilityBase.h"
#include "GameplayAbilityBase_ActiveAbility.generated.h"

class AGameplayAbilityTargetActor;
class UInputAction;

/**
 * 主动技能基类，通常用于玩家角色。
 *	- 主动释放的技能包括攻击、格挡、闪避等技能。
 *	- 技能分为四个阶段，前摇、生效开始、生效结束、后摇。
 *	- 绑定了输入的技能可以使用指令队列
 */
UCLASS(Abstract)
class SEKIROLIKE_API UGameplayAbilityBase_ActiveAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

protected:
	//////////////////////////////////////////
	/// TargetActor
	//////////////////////////////////////////
	/** 技能可能会用到的TargetActor */
	UPROPERTY(BlueprintReadWrite, Category="SekiroLike|Ability")
	AGameplayAbilityTargetActor* TargetActor;

	/** 技能可能会用到的TargetActor */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="SekiroLike|Ability")
	AGameplayAbilityTargetActor* GetTargetActor();

public:
#pragma region HELPER_FUNCTIONS
	/** 主动结束技能 */
	UFUNCTION()
	void EndAbilityActively()
	{
		constexpr bool bReplicateEndAbility = false;
		constexpr bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
	}

	/** 被动结束技能（被打断施放） */
	UFUNCTION()
	void EndAbilityPassively()
	{
		constexpr bool bReplicateEndAbility = false;
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility);
	}
#pragma endregion

	/** 技能标签 */
	static FGameplayTag ABILITY_TAG;

	//////////////////////////////////////////
	/// 输入
	//////////////////////////////////////////
	/** 技能是否绑定到输入 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Input")
	bool bUseActionBinding = false;
	
	/** 主动技能对应的输入，需要使用ASC的InputRelease回调时才填写 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Input", meta=(EditCondition="bUseActionBinding"))
	UInputAction* InputAction;
	
	/** 技能输入id，需要使用ASC的InputRelease回调时才填写
	 *  - 输入id与InputAction要求是一对一的关系，并且值不为INDEX_NONE时才视为有效。
	 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Input", meta=(EditCondition="bUseActionBinding"))
	int32 InputId = INDEX_NONE;
	/** 绑定的触发方式，仅可以选择Started和Triggered。按键松开默认会绑定。 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Input", meta=(EditCondition="bUseActionBinding"))
	ETriggerEvent TriggerEvent;

	UGameplayAbilityBase_ActiveAbility();

	/** 赋予ASC技能并绑定输入的工具方法 */
	static void GiveAbilityTo(TSubclassOf<UGameplayAbilityBase_ActiveAbility> AbilityClass, UAbilitySystemComponent* ASC, UEnhancedInputComponent* EnhancedInputComponent);

	//~ UGameplayAbility
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
