// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "UObject/Object.h"
#include "AbilityTask_WaitForNotify.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWaitNotifyDelegate, FName, NotifyName, const FAnimNotifyEvent&, NotifyEvent);

/**
 * 
 */
UCLASS()
class SEKIROLIKE_API UAbilityTask_WaitForNotify : public UAbilityTask
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UAnimInstance* AnimInstance;
	FName NotifyName;

	/////////////////////////////////////////////
	// 通知委托的回调方法
	UFUNCTION()
	void NotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);
	UFUNCTION()
	void NotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& Payload);
	
public:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	/**
	 * 等待动画通知任务。
	 * AnimInstance 默认取技能的ActorInfo的动画实例。
	 * NotifyName 指定了有效值时，仅当对应名称的通知触发时才广播 Notify相关委托。
	 */
	UFUNCTION(BlueprintCallable, Category="TD Ability|Tasks",
		meta=(HidePin="OwingAbility", DefaultToSelf="OwingAbility", BlueprintInternalUseOnly="TRUE"))
	static UAbilityTask_WaitForNotify* Create_WaitForNotify(
		UGameplayAbility* OwingAbility,
		FName TaskInstanceName,
		FName InNotifyName = NAME_None,
		UAnimInstance* InAnimInstance = nullptr);

	/////////////////////////////////////////////
	// 输出委托
	UPROPERTY(BlueprintAssignable)
	FWaitNotifyDelegate OnNotifyBegin;
	UPROPERTY(BlueprintAssignable)
	FWaitNotifyDelegate OnNotifyEnd;

	/////////////////////////////////////////////
	// 技能委托

	void OnAbilityCancelled();

	FDelegateHandle AbilityCancelledDelegate;
};

