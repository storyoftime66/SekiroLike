// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/ActorComponent.h"
#include "PlayerFocusComp.generated.h"


class UWidgetComponent;
class UCharacterMovementComponent;
class UInputAction;
class UUserWidget;

/**
 * 锁定敌人功能的组件
 *	- 可左右切换锁定的敌人
 *	
 * CheckList:
 *	- 使用前调用InitializeFocusComp()进行初始化
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType)
class SEKIROLIKE_API UPlayerFocusComp : public UActorComponent
{
	GENERATED_BODY()

	/** 当前锁定的敌人 */
	TWeakObjectPtr<ACharacter> FocusedCharacter;

	/** 拥有者的玩家控制器的引用 */
	TWeakObjectPtr<APlayerController> CompOwnerPC;

	/** 拥有者角色移动组件的引用，用于锁定时改变角色移动速度 */
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovement;

	/** 忽略视角旋转输入 */
	bool bIgnoreLookInput = false;

	/** 指示器组件，附加在FocusedCharacter上的 */
	UPROPERTY()
	UWidgetComponent* ReticleWidgetComp;

	bool bInitialized = false;

	/** Helper function，球形扫描
	 *  @param OutHits		输出结果
	 *  @param Start		检测起点
	 *  @param DirVector	检测方向向量，需要带长度
	 *  @return				是否检测到物体 */
	bool SphereSweepMulti(TArray<FHitResult>& OutHits, FVector Start, FVector DirVector) const;

protected:
	//~ UActorComponent
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	////////////////////////////
	/// 可配置参数

	/** 检测距离，默认10米 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Focus")
	float SweepDistance = 1500.0f;

	/** 最远保持锁定的距离的平方，默认25米 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Focus")
	float MaxDistanceSquare = 6250000.0f;

	/** 锁定目标的类型 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Focus")
	TEnumAsByte<ECollisionChannel> FocusObject = ECC_Pawn;

	/** 输入操作：锁定 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Focus")
	UInputAction* IA_Focus;

	/** 输入操作：切换锁定目标 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Focus")
	UInputAction* IA_SwitchTarget;

	/** 切换目标时扫描的最大角度，单位度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Focus", AdvancedDisplay, meta=(ClampMax="90.0", ClampMin="0.0"))
	float SwitchSweepAngle = 30.0f;

	/** 瞄准时的标识UI类 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SekiroLike|Focus", AdvancedDisplay)
	TSubclassOf<UUserWidget> ReticleClass;

	/** 设置新的锁定目标 */
	void SetFocusedCharacter(ACharacter* NewCharacter);

public:
	UPlayerFocusComp();

	/** 当前是否锁定到敌人身上 */
	bool IsFocused() const;
	
	/** 获取锁定的角色 */
	ACharacter* GetFocusedCharacter() const;

	/** 获取锁定IA */
	UInputAction* GetFocusAction() const { return IA_Focus; }

	/** 获取切换目标IA */
	UInputAction* GetSwitchTargetAction() const { return IA_SwitchTarget; }

	////////////////////////////////////
	/// 输入操作的处理方法

	/** 锁定和解除锁定 */
	UFUNCTION()
	void Focus();

	/** 切换锁定目标 */
	UFUNCTION()
	void SwitchTarget(const FInputActionValue& Value);
};
