// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameplayTagContainer.h"
#include "PlayerCharacter.generated.h"

class UPlayerFocusComp;
class UCameraComponent;
class USpringArmComponent;
struct FInputActionValue;
class UInputMappingContext;
class UGameplayAbilityBase_ActiveAbility;

UCLASS()
class SEKIROLIKE_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	/** 跟随的弹簧臂 */
	UPROPERTY()
	USpringArmComponent* FollowSpringArm;
	/** 跟随的相机 */
	UPROPERTY()
	UCameraComponent* FollowCamera;
	/** 锁定组件 */
	UPROPERTY(VisibleAnywhere, Category="SekiroLike|Components", BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UPlayerFocusComp* FocusComp;

protected:
	/** 初始主动技能组 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Abilities")
	TArray<TSubclassOf<UGameplayAbilityBase_ActiveAbility>> InitialActiveAbilities;

	FVector MovementInput;

#pragma region PLAYER_INPUT

	////////////////////////////////////////
	/// 可用的输入动作
	UPROPERTY(EditDefaultsOnly, Category="SekiroLike|Input")
	UInputMappingContext* IMC_Default;
	UPROPERTY(EditDefaultsOnly, Category="SekiroLike|Input")
	UInputAction* IA_Move;
	UPROPERTY(EditDefaultsOnly, Category="SekiroLike|Input")
	UInputAction* IA_Look;

	////////////////////////////////////////
	/// 部分动作对应的技能tag
	// /** 攻击动作的技能tag */
	// UPROPERTY(EditAnywhere, Category="SekiroLike|Input")
	// FGameplayTag AbilityTag_Attack;
	// /** 上一个使用的技能的tag */
	// FGameplayTag LastAbilityTag;

	////////////////////////////////////////
	/// 输入动作的处理函数
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Debug(const FInputActionValue& Value);

#pragma endregion

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

#pragma region HELPER_FUNCTIONS
	/** 应用新的输入映射 */
	void ApplyInputMappingContext(const UInputMappingContext* NewMappingContext, int32 Priority) const;
	/** 重新映射技能到输入操作 */
	void RemapAbilityToInputAction();
#pragma endregion

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


	//~ IMovementAgentInterface
	virtual FVector GetWorldMovementIntent() const override;

	UFUNCTION(BlueprintCallable, Category="SekiroLike|Debug")
	void DebugAddGameplayTag(FGameplayTag DebugTag);
};
