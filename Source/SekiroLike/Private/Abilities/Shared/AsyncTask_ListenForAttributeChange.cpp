// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/Shared/AsyncTask_ListenForAttributeChange.h"

#include "AbilitySystemComponent.h"


UAsyncTask_ListenForAttributeChange* UAsyncTask_ListenForAttributeChange::ListenForAttributeChange(
	UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
	UAsyncTask_ListenForAttributeChange* Task = NewObject<UAsyncTask_ListenForAttributeChange>();
	Task->ASC = AbilitySystemComponent;
	Task->AttributeToListenFor = Attribute;

	if (!IsValid(AbilitySystemComponent) || !Attribute.IsValid())
	{
		// Figure Out: RemoveFromRoot() 作用。
		Task->RemoveFromRoot();
		return nullptr;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute)
	                      .AddUObject(Task, &UAsyncTask_ListenForAttributeChange::OnAttributeChanged);

	return Task;
}

UAsyncTask_ListenForAttributeChange* UAsyncTask_ListenForAttributeChange::ListenForAttributesChange(
	UAbilitySystemComponent* AbilitySystemComponent, TArray<FGameplayAttribute> Attributes)
{
	UAsyncTask_ListenForAttributeChange* Task = NewObject<UAsyncTask_ListenForAttributeChange>();
	Task->ASC = AbilitySystemComponent;
	Task->AttributesToListenFor = Attributes;

	if (!IsValid(AbilitySystemComponent) || Attributes.Num() < 1)
	{
		Task->RemoveFromRoot();
		return nullptr;
	}

	for (const FGameplayAttribute Attribute : Attributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute)
		                      .AddUObject(Task, &UAsyncTask_ListenForAttributeChange::OnAttributeChanged);
	}

	return Task;
}

void UAsyncTask_ListenForAttributeChange::EndTask()
{
	if (IsValid(ASC))
	{
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);

		for (const FGameplayAttribute Attribute : AttributesToListenFor)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
	MarkPendingKill();
}

void UAsyncTask_ListenForAttributeChange::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	AttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}
