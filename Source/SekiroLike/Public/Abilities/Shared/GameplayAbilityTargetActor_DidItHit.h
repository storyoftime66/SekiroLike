// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_DidItHit.generated.h"

class UDidItHitActorComponent;

/**
 * DidItHit TargetActor。
 * - 实际上可以用普通的Actor来使用DidItHitComp。
 */
UCLASS(BlueprintType)
class SEKIROLIKE_API AGameplayAbilityTargetActor_DidItHit : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

protected:
	/** 检测组件 */
	UPROPERTY(BlueprintReadOnly, Category="SekiroLike|Ability")
	UDidItHitActorComponent* DidItHitComp;

	/** 是否正在检测目标 */
	UPROPERTY()
	bool bIsTargeting = false;

public:
	// Sets default values for this actor's properties
	AGameplayAbilityTargetActor_DidItHit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FGameplayAbilityTargetDataHandle MakeTargetData(FHitResult& HitResult);
	
	UFUNCTION()
	void OnHitActorHandler(FHitResult HitResult);

public:
	//~ AGameplayAbilityTargetActor
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
	virtual bool IsConfirmTargetingAllowed() override;

	/** 工具方法，创建TargetActor */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|Ability")
	static AGameplayAbilityTargetActor_DidItHit* MakeTargetActor_DidItHit(ACharacter* AbilityAvatar);
};
