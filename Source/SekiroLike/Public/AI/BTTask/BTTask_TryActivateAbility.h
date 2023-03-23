// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_TryActivateAbility.generated.h"

class USLAbilitySystemComponent;

struct FBTWaitAbilityEnded
{
	/** 剩余时间 */
	float RemainingWaitTime;
	bool bAbilityEnded;
	bool bAbilityCancelled;
	TWeakObjectPtr<USLAbilitySystemComponent> ASC;
};

/**
 * AI任务：尝试激活技能
 */
UCLASS()
class SEKIROLIKE_API UBTTask_TryActivateAbility : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
protected:
	/** 是否为非阻塞任务 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	bool NonBlock = false;
	
	/** 技能类 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	TSubclassOf<class UGameplayAbility> AbilityClass;

	/** 超时时间，单位：秒 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	float TimeOut = 10.0f;
	
public:
	UBTTask_TryActivateAbility();

	//~ UBTTaskNode
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

public:
	virtual FString GetStaticDescription() const override;
	
};
