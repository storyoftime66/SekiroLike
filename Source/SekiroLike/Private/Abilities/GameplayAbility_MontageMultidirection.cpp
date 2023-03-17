// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_MontageMultidirection.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitForNotify.h"

#include "Characters/MovementAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"

UGameplayAbility_MontageMultidirection::UGameplayAbility_MontageMultidirection()
{
}

UAnimMontage* UGameplayAbility_MontageMultidirection::GetAbilityMontage_Implementation()
{
	UAnimMontage* MontageToPlay = nullptr;

	// 得到移动方向在角色本地坐标空间的旋转Yaw方向，用以确定使用的蒙太奇
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	IMovementAgentInterface* MovementAgent = CastChecked<IMovementAgentInterface>(AvatarActor);
	FVector MovementIntent = MovementAgent->GetWorldMovementIntent();
	MovementIntent.Z = 0.0f;
	if (!MovementIntent.IsNearlyZero(0.0001f))
	{
		float ActionDirection = (UKismetMathLibrary::MakeRotFromX(MovementIntent) - AvatarActor->GetActorRotation()).Yaw + 360.0f; // 将角度变成正数
		if (bFourDirection)
		{
			float Section = 90.0f;
			int32 DirectionNum = 4;
			const int32 Selection = FMath::RoundToInt(ActionDirection / Section) % DirectionNum;
			// Note: switch不好看
			if (Selection == 0) MontageToPlay = Montage_F;
			if (Selection == 1) MontageToPlay = Montage_R;
			if (Selection == 2) MontageToPlay = Montage_B;
			if (Selection == 3) MontageToPlay = Montage_L;
		}
		else
		{
			float Section = 45.0f;
			int32 DirectionNum = 8;
			const int32 Selection = FMath::RoundToInt(ActionDirection / Section) % DirectionNum;
			if (Selection == 0) MontageToPlay = Montage_F;
			if (Selection == 1) MontageToPlay = Montage_RF;
			if (Selection == 2) MontageToPlay = Montage_R;
			if (Selection == 3) MontageToPlay = Montage_RB;
			if (Selection == 4) MontageToPlay = Montage_B;
			if (Selection == 5) MontageToPlay = Montage_LB;
			if (Selection == 6) MontageToPlay = Montage_L;
			if (Selection == 7) MontageToPlay = Montage_LF;
		}
	}
	else
	{
		// 无移动时默认向前
		MontageToPlay = Montage_F;
	}

	return MontageToPlay;
}

void UGameplayAbility_MontageMultidirection::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		return;
	}

	// 先执行蓝图的逻辑
	if (bHasBlueprintActivate)
	{
		K2_ActivateAbility();
	}
	else if (bHasBlueprintActivateFromEvent)
	{
		K2_ActivateAbilityFromEvent(*TriggerEventData);
	}

	check(GetAvatarActorFromActorInfo());

	auto MontageToPlay = GetAbilityMontage();

	// TODO: 在不同阶段给角色添加不同的tag
	static FName MontageTaskName("PlayMontage_In_MontageMultidirection");
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, MontageToPlay, 1.0f, NAME_None, false);
	MontageTask->OnCancelled.AddDynamic(this, &UGameplayAbility_MontageMultidirection::EndAbilityPassively);
	MontageTask->OnInterrupted.AddDynamic(this, &UGameplayAbility_MontageMultidirection::EndAbilityActively);
	MontageTask->OnCompleted.AddDynamic(this, &UGameplayAbility_MontageMultidirection::EndAbilityActively);

	MontageTask->ReadyForActivation();
}
