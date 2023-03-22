// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Shared/GameplayAbilityTargetActor_PresetTarget.h"


// Sets default values
AGameplayAbilityTargetActor_PresetTarget::AGameplayAbilityTargetActor_PresetTarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ShouldProduceTargetDataOnServer = true;
	bDestroyOnConfirmation = true;
}

AGameplayAbilityTargetActor_PresetTarget* AGameplayAbilityTargetActor_PresetTarget::MakeTargetActor_PresetTarget_FromActor(AActor* TargettingActor)
{
	if (!IsValid(TargettingActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor_PresetTarget spawned failed becauce of invalid WorldContext."));
		return nullptr;
	}

	auto TargetActor = TargettingActor->GetWorld()->SpawnActor<AGameplayAbilityTargetActor_PresetTarget>(TargettingActor->GetActorLocation(), FRotator::ZeroRotator);
	if (TargetActor)
	{
		TargetActor->AddTarget(TargettingActor);
	}
	return TargetActor;
}

AGameplayAbilityTargetActor_PresetTarget* AGameplayAbilityTargetActor_PresetTarget::MakeTargetActor_PresetTarget_FromActors(TArray<AActor*> TargettingActors)
{
	if (TargettingActors.Num() == 0 or !IsValid(TargettingActors[0]))
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor_PresetTarget spawned failed becauce of invalid WorldContext."));
		return nullptr;
	}

	auto TargetActor = TargettingActors[0]->GetWorld()->SpawnActor<AGameplayAbilityTargetActor_PresetTarget>(TargettingActors[0]->GetActorLocation(), FRotator::ZeroRotator);
	if (TargetActor)
	{
		TargetActor->AddTargets(TargettingActors);
	}
	return TargetActor;
}

void AGameplayAbilityTargetActor_PresetTarget::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
}

void AGameplayAbilityTargetActor_PresetTarget::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (IsConfirmTargetingAllowed())
	{
		// Note: MakeTargetDataHandleFromActors会自动筛选有效的Actor
		FGameplayAbilityTargetDataHandle TargetDataHandle = StartLocation.MakeTargetDataHandleFromActors(Targets, false);
		TargetDataReadyDelegate.Broadcast(TargetDataHandle);
	}
}
