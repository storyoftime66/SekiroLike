// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitTargetDataUsingActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitTargetDataUsingActorDelegate, const FGameplayAbilityTargetDataHandle&, Data);

/**
 * 和UAbilityTask_WaitTargetData差不多
 */
UCLASS()
class SEKIROLIKE_API UAbilityTask_WaitTargetDataUsingActor : public UAbilityTask
{
	GENERATED_BODY()

public:

	/** 使用的TargetActor */
	UPROPERTY()
	AGameplayAbilityTargetActor* TargetActor;
	
	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataUsingActorDelegate ValidData;

	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataUsingActorDelegate Cancelled;

	UFUNCTION()
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data);

	FDelegateHandle ReadyDelegateHandle;
	FDelegateHandle CancelledDelegateHandle;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "SekiroLike|Ability")
	static UAbilityTask_WaitTargetDataUsingActor* WaitTargetDataUsingActor(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		AGameplayAbilityTargetActor* InTargetActor);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
};
