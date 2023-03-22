// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Abilities/SLAbilityTypes.h"
#include "AnimNotifyState_SetAbilityStage.generated.h"

/**
 * 设置角色所处的技能阶段，结束时移除
 */
UCLASS()
class SEKIROLIKE_API UAnimNotifyState_SetAbilityStage : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	/** 需要设置的技能阶段Tag */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	EAbilityStage AbilityStage;
	/** 在状态开始时是否通知Actor */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	bool bNotifyActorAtBeginning = true;
	/** 在状态结束时是否移除tag */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	bool bRemoveTagAtEnding = true;

	/** 缓存的MeshComp的拥有者 */
	UPROPERTY()
	AActor* AvatarActor;

	/** 技能标签 */
	FGameplayTag AbilityStageTag;

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
