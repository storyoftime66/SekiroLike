// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/GameFrameworkComponent.h"
#include "PlayerFocusComp.generated.h"


class UWidgetComponent;
class UCharacterMovementComponent;
class UInputAction;
class UUserWidget;

/**
 * 锁定敌人功能的组件
 *	- 可左右切换锁定的敌人
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType)
class SEKIROLIKE_API UPlayerFocusComp : public UGameFrameworkComponent
{
	GENERATED_BODY()

	/** 当前锁定的敌人 */
	TWeakObjectPtr<ACharacter> TargetCharacter;

	/** 拥有者的玩家控制器的引用 */
	TWeakObjectPtr<APlayerController> CompOwnerPC;

	/** 拥有者角色移动组件的引用，用于锁定时改变角色移动速度 */
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovement;

	/** 忽略视角旋转输入 */
	bool bIgnoreLookInput = false;

	/** 锁定目标的碰撞通道类型 */
	TEnumAsByte<ECollisionChannel> FocusObject = ECC_Pawn;

	/** 指示器组件，附加在TargetCharacter上的 */
	UPROPERTY()
	UWidgetComponent* ReticleWidgetComp;

	/** Helper function，球形扫描
	 *  @param OutHits		输出结果
	 *  @param Start		检测起点
	 *  @param DirVector	检测方向向量，需要带长度
	 *  @return				是否检测到物体 */
	bool SphereSweepMulti(TArray<FHitResult>& OutHits, FVector Start, FVector DirVector) const;

protected:
	//~ Begin UActorComponent
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent

	////////////////////////////
	/// 可配置参数

	/** 检测距离，默认15米 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Focus")
	float SweepDistance = 1500.0f;

	/** 最远保持锁定的距离的平方，默认25米 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Focus")
	float MaxDistanceSquare = 6250000.0f;

	/** 输入操作：切换锁定 */
	UPROPERTY(EditAnywhere, Category = "SekiroLike|Focus")
	UInputAction* IA_ToggleFocus;

	/** 输入操作：切换锁定目标 */
	UPROPERTY(EditAnywhere, Category = "SekiroLike|Focus")
	UInputAction* IA_SwitchFocusTarget;

	/** 切换目标时扫描的最大角度，单位度 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Focus", AdvancedDisplay, meta=(ClampMax="90.0", ClampMin="0.0"))
	float SwitchSweepAngle = 30.0f;

	/** 瞄准时的标识UI类 */
	UPROPERTY(EditAnywhere, Category="SekiroLike|Focus", AdvancedDisplay)
	TSubclassOf<UUserWidget> ReticleClass;

	/** 设置新的锁定目标 */
	void SetTargetCharacter(ACharacter* NewCharacter);

public:
	UPlayerFocusComp(const FObjectInitializer& ObjectInitializer);

	/** 当前是否锁定到敌人身上 */
	UFUNCTION(BlueprintCallable, Category = "SekiroLike|Focus")
	bool IsTargeting() const;
	
	/** 获取锁定的角色 */
	UFUNCTION(BlueprintCallable, Category = "SekiroLike|Focus")
	ACharacter* GetTargetCharacter() const;

	/** 绑定输入，应在SetupPlayerInputComponent中调用 */
	void BindInput(UEnhancedInputComponent* EnhancedInputComp);

	////////////////////////////////////
	/// 输入操作的处理方法

	/** 锁定和解除锁定 */
	UFUNCTION()
	void ToggleFocus();

	/** 切换锁定目标 */
	UFUNCTION()
	void SwitchFocusTarget(const FInputActionValue& Value);
};
