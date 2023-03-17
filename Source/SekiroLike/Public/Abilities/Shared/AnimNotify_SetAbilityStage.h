// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SLAbilityTypes.h"
#include "AnimNotify_SetAbilityStage.generated.h"

/**
 * 设置角色所处的技能阶段。（弃用：请使用AnimNotifyState_SetAbilityStage）
 */
UCLASS()
class SEKIROLIKE_API UAnimNotify_SetAbilityStage : public UAnimNotify
{
	GENERATED_BODY()

protected:
	/** 需要设置的技能阶段Tag，为空时则会清空角色身上的技能阶段Tag */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	EAbilityStage AbilityStage;
	/** 是否通知Actor */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	bool bNotifyActor = true;

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
