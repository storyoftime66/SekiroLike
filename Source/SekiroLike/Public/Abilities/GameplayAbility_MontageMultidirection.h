// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase_ActiveAbility.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_MontageMultidirection.generated.h"

class UAnimMontage;

/**
 * 八方向/四方向的技能，每个方向有其独特的技能动画，但效果相同。
 * - 不移动时默认播放向前的动画。
 * - 适用于翻滚这类技能。
 */
UCLASS()
class SEKIROLIKE_API UGameplayAbility_MontageMultidirection : public UGameplayAbilityBase_ActiveAbility
{
	GENERATED_BODY()

public:
	UGameplayAbility_MontageMultidirection();

protected:
	/** 是否仅使用前、后、左、右四个方向 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	bool bFourDirection = false;

#pragma region ANIMATIONS
	// Note: 写成这样而不写成数组是为了配的时候方便一些

	/** 向前的动画 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	UAnimMontage* Montage_F;
	/** 向后的动画 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	UAnimMontage* Montage_B;
	/** 向左的动画 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	UAnimMontage* Montage_L;
	/** 向右的动画 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	UAnimMontage* Montage_R;
	/** 向左前的动画 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	UAnimMontage* Montage_LF;
	/** 向左后的动画 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	UAnimMontage* Montage_LB;
	/** 向右前的动画 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	UAnimMontage* Montage_RF;
	/** 向右后的动画 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	UAnimMontage* Montage_RB;
#pragma endregion

	//~ GameplayAbilityBase
	virtual UAnimMontage* GetAbilityMontage_Implementation() override;

	//~ GameplayAbility
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
