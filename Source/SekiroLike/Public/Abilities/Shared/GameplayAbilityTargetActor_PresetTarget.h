// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_PresetTarget.generated.h"


/**
 * 预先设置好目标的TargetActor，仅用来等待玩家输入确认
 */
UCLASS()
class SEKIROLIKE_API AGameplayAbilityTargetActor_PresetTarget : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> Targets;

public:
	// Sets default values for this actor's properties
	AGameplayAbilityTargetActor_PresetTarget();

	UFUNCTION(BlueprintPure, Category="SekiroLike|TargetData")
	static AGameplayAbilityTargetActor_PresetTarget* MakeTargetActor_PresetTarget_FromActor(AActor* TargettingActor);

	UFUNCTION(BlueprintPure, Category="SekiroLike|TargetData")
	static AGameplayAbilityTargetActor_PresetTarget* MakeTargetActor_PresetTarget_FromActors(TArray<AActor*> TargettingActors);

	/** 添加作为目标的Actor */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|TargetData")
	void AddTarget(AActor* TargetingActor) { Targets.Add(TargetingActor); }

	/** 添加多个作为目标的Actor */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|TargetData")
	void AddTargets(TArray<AActor*> TargetingActors) { Targets.Append(TargetingActors); }

	//~ AGameplayAbilityTargetActor
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
};
