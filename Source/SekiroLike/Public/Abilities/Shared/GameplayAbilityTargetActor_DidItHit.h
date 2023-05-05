// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_DidItHit.generated.h"

class UDidItHitActorComponent;

/**
 * DidItHit TargetActor。
 * - 实际上可以用普通的Actor来使用DidItHitComp。
 * - 不需要玩家确认，也不会去绑定ASC的确认委托。
 */
UCLASS(BlueprintType)
class SEKIROLIKE_API AGameplayAbilityTargetActor_DidItHit : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

protected:
	/** 检测组件 */
	UPROPERTY(BlueprintReadOnly, Category="SekiroLike|Ability")
	UDidItHitActorComponent* DidItHitComp;

	/** 是否正在检测目标，用于控制DidItHit组件。 */
	UPROPERTY()
	bool bIsTargeting = false;

public:
	AGameplayAbilityTargetActor_DidItHit();

protected:
	virtual void BeginPlay() override;

	FGameplayAbilityTargetDataHandle MakeTargetData(FHitResult& HitResult);

	UFUNCTION()
	void OnHitActorHandler(FHitResult HitResult);

public:
	//~ AGameplayAbilityTargetActor
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
	virtual bool IsConfirmTargetingAllowed() override;

	/** 工具方法，创建TargetActor
	 * @param AbilityAvatar 技能的Avatar
	 * @param TraceChannel 检测通道
	 * @param bAttachToAvatar 是否连接到 AbilityAvatar 上，默认为是
	 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|TargetData")
	static AGameplayAbilityTargetActor_DidItHit* MakeTargetActor_DidItHit(
		ACharacter* AbilityAvatar,
		TEnumAsByte<ETraceTypeQuery> TraceChannel,
		bool bAttachToAvatar = true);
};
