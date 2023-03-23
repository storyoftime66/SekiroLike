// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityAvatarInterface.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "Characters/Components/SLAbilitySystemComponent.h"
#include "Components/TimelineComponent.h"

#include "GenericTeamAgentInterface.h"
#include "MovementAgentInterface.h"
#include "GameFramework/Character.h"

#include "BaseCharacter.generated.h"

class UGameplayEffect;
class UGameplayAbilityBase_ActiveAbility;
class UGameplayAbility;
class UCharAttributeSet;
class UInputAction;

UCLASS()
class SEKIROLIKE_API ABaseCharacter :
	public ACharacter,
	public IAbilitySystemInterface,
	public IMovementAgentInterface,
	public IAbilityAvatarInterface,
	public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	/** TODO: 临时用于解决BlockTags问题的变量  */
	float TimeAccumulation = 0.0f;

	////////////////////////////////////////////////////////////
	/// 技能系统
	////////////////////////////////////////////////////////////
protected:
	/** 技能组件和属性集 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USLAbilitySystemComponent* ASC;
	UPROPERTY()
	UCharAttributeSet* CharAS;
	/** 初始技能组 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	TArray<TSubclassOf<UGameplayAbility>> InitialAbilities;
	/** 初始效果（包括初始属性） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Ability")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	/** 初始激活的技能的SpecHandles */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> PersistentSpecHandles;

public:
	//~ IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }

	////////////////////////////////////////////////////////////
	/// MovementAgent
	////////////////////////////////////////////////////////////
protected:
	/** 行走模式 */
	EWalkMode WalkMode = Run;
	/** 走路状态速度 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Character")
	float WalkSpeed = 120.0f;
	/** 奔跑状态速度 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Character")
	float RunSpeed = 450.0;

public:
	//~ IMovementAgentInterface
	virtual void SetWalkMode(EWalkMode NewWalkMode) override;
	virtual EWalkMode GetWalkMode() const override;
	virtual FVector GetWorldMovementIntent() const override;
	virtual FVector GetLocalMovementIntent() const override;

	////////////////////////////////////////////////////////////
	/// AbilityAvatar
	////////////////////////////////////////////////////////////
protected:
	/** 受击时的物理动画权重曲线 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Character")
	UCurveFloat* ReactToHitCurve;

	/** 物理动画时间轴 */
	FTimeline ReactToHitTimeline;

	/** 物理动画结束回调 */
	FOnTimelineEvent OnReactToHitFinished;

	/** 受击的骨骼名称 */
	FName HitBoneName;

	/** 当前姿势类型，分为左手姿势和右手姿势 */
	EPostureType PostureType = EPostureType::None;

	/** 角色是否已死亡 */
	bool IsCharacterDead = false;

	/** 物理动画混合权重时间轴函数 */
	UFUNCTION()
	void ReactToHitTimelineProgress(float Value);

	/** 物理动画结束回调函数 */
	UFUNCTION()
	void ReactToHitFinished();

public:
	//~ IAbilityAvatarInterface
	virtual void AddAbilityStage(const FGameplayTag& AbilityStageTag) override;
	virtual void ClearAbilityStage() override;
	virtual float GetAheadFoot() const override;
	virtual EPostureType GetPostureType() const override { return PostureType; }
	virtual void SetPostureType(EPostureType NewPostureType) override { PostureType = NewPostureType; }
	virtual void ReactToHit(FName BoneName, FVector HitImpulse) override;
	virtual void NotifyDied(AActor* InInstigator) override;
	virtual bool IsDead() override { return IsCharacterDead; }

protected:
	/** 善后工作 */
	UFUNCTION(BlueprintNativeEvent, Category="SekiroLike|Character")
	void Died(AActor* InInstigator);

public:
	//~ IGenericTeamAgentInterface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

public:
	ABaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void PostInitializeComponents() override;
};
