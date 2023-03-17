// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PlayMontageAndWaitForNotify.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMontageAndWaitForNotifyDelegate, FName, NotifyName, const FAnimNotifyEvent&, NotifyEvent);


/**
 * 
 */
UCLASS()
class SEKIROLIKE_API UAbilityTask_PlayMontageAndWaitForNotify : public UAbilityTask
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UAnimInstance* AnimInstance;
	UPROPERTY()
	UAnimMontage* Montage;

	/** 通知名称，为None时会广播所有通知 */
	FName NotifyName;
	bool bStopWhenAbilityEnds;

	/** 停止播放蒙太奇 */
	bool StopPlayingMontage();

public:
	//~ UAbilityTask
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~ End UAbilityTask

	/**
	 * 播放蒙太奇并等待通知。
	 * AnimInstance默认取技能的ActorInfo的动画实例。
	 * NotifyName设置了有效值时，仅当对应名称的通知触发时才广播Notify委托。
	 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|Ability", meta=(HidePin="OwingAbility", DefaultToSelf="OwingAbility", BlueprintInternalUseOnly="TRUE"))
	static UAbilityTask_PlayMontageAndWaitForNotify* Create_PlayMontageAndWaitForNotifyTask(
		UGameplayAbility* OwingAbility,
		FName TaskInstanceName,
		UAnimMontage* MontageToPlay,
		UAnimInstance* InAnimInstance = nullptr,
		FName InNotifyName = NAME_None,
		bool StopWhenAbilityEnds = true);

	/////////////////////////////////////////////
	// 输出引脚
	/** 通知开始 */
	UPROPERTY(BlueprintAssignable)
	FMontageAndWaitForNotifyDelegate OnNotifyBegin;
	/** 通知结束 */
	UPROPERTY(BlueprintAssignable)
	FMontageAndWaitForNotifyDelegate OnNotifyEnd;
	/** 技能被取消 */
	UPROPERTY(BlueprintAssignable)
	FMontageAndWaitForNotifyDelegate OnCancelled;
	/** 蒙太奇中断 */
	UPROPERTY(BlueprintAssignable)
	FMontageAndWaitForNotifyDelegate OnInterrupted;
	/** 蒙太奇混出 */
	UPROPERTY(BlueprintAssignable)
	FMontageAndWaitForNotifyDelegate OnBlendOut;
	/** 蒙太奇完成 */
	UPROPERTY(BlueprintAssignable)
	FMontageAndWaitForNotifyDelegate OnCompleted;

protected:
	/////////////////////////////////////////////
	// 动画通知Handler
	UFUNCTION()
	void NotifyBegin(FName InNotifyName, const FBranchingPointNotifyPayload& Payload);
	UFUNCTION()
	void NotifyEnd(FName InNotifyName, const FBranchingPointNotifyPayload& Payload);
	
	/////////////////////////////////////////////
	// 技能回调Handler
	void AbilityCancelled();
	FDelegateHandle AbilityCancelledDelegate;

	/////////////////////////////////////////////
	// 蒙太奇回调所需的委托和Handler
	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	void MontageEnded(UAnimMontage* InMontage, bool bInterrupted);
	void MontageBlendingOut(UAnimMontage* InMontage, bool bInterrupted);
};
