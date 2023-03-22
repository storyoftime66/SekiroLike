// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_TryActivateAbility.h"

#include "AbilitySystemInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Characters/Components/SLAbilitySystemComponent.h"

UBTTask_TryActivateAbility::UBTTask_TryActivateAbility()
{
	NodeName = "Try Activate Ability";
	bNotifyTick = true;

	// 使用技能的Actor
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TryActivateAbility, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_TryActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	AActor* AvatarActor = nullptr;
	if (MyBlackboard && BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		AvatarActor = Cast<AActor>(MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID()));
	}

	const auto AbilitySystemInterface = Cast<IAbilitySystemInterface>(AvatarActor);
	if (AbilitySystemInterface == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const auto ASC = Cast<USLAbilitySystemComponent>(AbilitySystemInterface->GetAbilitySystemComponent());
	if (!IsValid(ASC))
	{
		return EBTNodeResult::Failed;
	}

	FBTWaitAbilityEnded* MyMemory = reinterpret_cast<FBTWaitAbilityEnded*>(NodeMemory);
	MyMemory->RemainingWaitTime = TimeOut;
	MyMemory->bAbilityEnded = false;
	MyMemory->bAbilityCancelled = false;
	MyMemory->ASC = ASC;
	ASC->AddSelfServiceQueryAbility(AbilityClass);

	const bool bSuccess = ASC->TryActivateAbilityByClass(AbilityClass, false);
	if (bSuccess)
	{
		if (NonBlock)
		{
			return EBTNodeResult::Succeeded;
		}
		else
		{
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_TryActivateAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTWaitAbilityEnded* MyMemory = reinterpret_cast<FBTWaitAbilityEnded*>(NodeMemory);

	if (MyMemory and IsValid(MyMemory->ASC))
	{
		MyMemory->bAbilityCancelled = MyMemory->ASC->IsAbilityCancelled(AbilityClass);
		if (MyMemory->bAbilityCancelled)
		{
			FinishLatentAbort(OwnerComp);
		}

		MyMemory->bAbilityEnded = MyMemory->ASC->IsAbilityEnded(AbilityClass);
		if (MyMemory->bAbilityEnded)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}

		MyMemory->RemainingWaitTime -= DeltaSeconds;
		if (MyMemory->RemainingWaitTime <= 0.0f)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	else
	{
		FinishLatentAbort(OwnerComp);
	}
}

void UBTTask_TryActivateAbility::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	FBTWaitAbilityEnded* MyMemory = reinterpret_cast<FBTWaitAbilityEnded*>(NodeMemory);
	if (MyMemory and IsValid(MyMemory->ASC))
	{
		MyMemory->ASC->RemoveSelfServiceQueryAbility(AbilityClass);
	}
}

FString UBTTask_TryActivateAbility::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}
