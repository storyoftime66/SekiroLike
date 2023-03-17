// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase_ActiveAbility.h"
#include "GameplayAbility_MontageCombo.generated.h"

/**
 * 连击技能，每段技能有其独特的技能动画。
 * 适用于普通攻击这样的技能。
 */
UCLASS()
class SEKIROLIKE_API UGameplayAbility_MontageCombo : public UGameplayAbilityBase_ActiveAbility
{
	GENERATED_BODY()

protected:
	/** 每段技能的动画 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	TArray<UAnimMontage*> ComboMontages;
	/** 维持连击的最大间隔时间 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	float MaxComboInterval = 1.2f;

	/////////////////////////////////////////////
	/// 自动连击
	/** 是否需要自动连击。
	 *  在释放前一段攻击时再次尝试激活该技能时，会尝试自动连击，即当次技能结束后立刻再次激活技能。
	 *  旨在优化连击技能的施放体验。 */
	bool bAutoCombo = false;
	/** 技能释放失败时的判断是否需要自动连击 */
	void OnAbilityFailed(const UGameplayAbility*, const FGameplayTagContainer&);
	FDelegateHandle AbilityFailedDelegateHandle;
	FTimerHandle TimerHandle_AutoCombo;

	/** 自动连击，会在技能结束时触发 */
	UFUNCTION(BlueprintNativeEvent)
	void ActivateForNextTick();

	/** 当前连击段数 */
	UPROPERTY(BlueprintReadOnly, Category="SekiroLike|Ability")
	int32 CurrentCombo = 0;
	
	/** 上次施放时间 */
	UPROPERTY(BlueprintReadOnly, Category="SekiroLike|Ability")
	float LastTimeActivated = -10000.0f;
	
	/** 上次按下时间 */
	UPROPERTY(BlueprintReadOnly, Category="SekiroLike|Ability")
	float LastTimePressed = -10000.0f;
	
	/** 上次松开时间 */
	UPROPERTY(BlueprintReadOnly, Category="SekiroLike|Ability")
	float LastTimeReleased = -10000.0f;

public:
	UGameplayAbility_MontageCombo();

protected:
	/** 重置连击 */
	UFUNCTION()
	void ResetCombo();

	//~ UGameplayAbility
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
};
