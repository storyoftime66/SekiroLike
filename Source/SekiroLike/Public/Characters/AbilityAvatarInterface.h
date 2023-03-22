// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "UObject/Interface.h"
#include "AbilityAvatarInterface.generated.h"

/** 当前姿势的类型
 *	- 主要分为左手动作、右手动作*/
UENUM(BlueprintType)
enum class EPostureType : uint8
{
	None,
	LeftHand,
	RightHand,
	Max
};


struct FGameplayTag;
// This class does not need to be modified.
UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UAbilityAvatarInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 会使用技能的角色需要实现的接口
 */
class SEKIROLIKE_API IAbilityAvatarInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	///////////////////////////
	/// 技能相关
	/** 添加角色技能阶段tag（前摇、生效、后摇） */
	virtual void AddAbilityStage(const FGameplayTag& AbilityStageTag)
	{
	}

	/** 清空角色技能阶段tag */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|AbilityAvatarInterface")
	virtual void ClearAbilityStage()
	{
	}

	//////////////////
	/// 动画相关
	/** 获取角色移动时在前面的脚
	 *  左脚在前时返回-1，双脚平行时返回0，右脚在前时返回1 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|AbilityAvatarInterface")
	virtual float GetAheadFoot() const { return 0.0f; }

	/** 获取当前姿势类型 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|AbilityAvatarInterface")
	virtual EPostureType GetPostureType() const { return EPostureType::None; }

	/** 设置当前姿势类型 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|AbilityAvatarInterface")
	virtual void SetPostureType(EPostureType NewPostureType)
	{
	}

	/** 获取物理动画组件，暂未使用 */
	virtual UPhysicalAnimationComponent* GetPhysicalAnimationComponent() const { return nullptr; }

	/** 对受击作出反应（播放物理动画） */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|AbilityAvatarInterface")
	virtual void ReactToHit(FName BoneName, FVector HitImpulse)
	{
	}
};


/**
 * 设置角色的姿势类型
 */
UCLASS()
class SEKIROLIKE_API UAnimNotifyState_PostureType : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	/** 需要设置的姿势类型 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Ability")
	EPostureType AnimPostureType = EPostureType::RightHand;

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
