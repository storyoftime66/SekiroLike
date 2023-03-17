// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncTask_ListenForAttributeChange.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttributeChangeDelegate, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

struct FOnAttributeChangeData;

/**
 * 异步任务：监听复数个属性。
 * Note：由于不是技能任务，所以可以在技能外使用（但需要手动结束任务）。常用于UI更新。
 *
 * 参考自GASShooter项目。
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy = AsyncTask))
class SEKIROLIKE_API UAsyncTask_ListenForAttributeChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

protected:
	void OnAttributeChanged(const FOnAttributeChangeData& Data);

	UPROPERTY()
	UAbilitySystemComponent* ASC;

	// 单个任务可同时监听多个属性
	FGameplayAttribute AttributeToListenFor;
	TArray<FGameplayAttribute> AttributesToListenFor;

public:
	UPROPERTY(BlueprintAssignable)
	FAttributeChangeDelegate AttributeChanged;

	/** 工厂方法：创建监听单个属性的任务 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|AsyncTasks", meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_ListenForAttributeChange* ListenForAttributeChange(
		UAbilitySystemComponent* InASC, FGameplayAttribute Attribute);

	/** 工厂方法：创建监听多个属性的任务 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|AsyncTasks", meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_ListenForAttributeChange* ListenForAttributesChange(
		UAbilitySystemComponent* InASC, TArray<FGameplayAttribute> Attributes);

	/** 不再监听时*必须*调用此方法。*/
	UFUNCTION(BlueprintCallable, Category="SekiroLike|AsyncTasks")
	void EndTask();
};
