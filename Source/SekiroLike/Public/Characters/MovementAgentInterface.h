// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MovementAgentInterface.generated.h"

/** 行走模式 */
UENUM(BlueprintType)
enum EWalkMode
{
	None = 0,
	Walk,
	Run,
	Max
};

// This class does not need to be modified.
UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UMovementAgentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 会移动的角色需要实现此接口
 */
class SEKIROLIKE_API IMovementAgentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//////////////////////
	/// 角色相关
	
	/** 设置行走模式 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|MovementAgent")
	virtual void SetWalkMode(EWalkMode NewWalkMode);

	/** 获取当前的行走模式 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|MovementAgent")
	virtual EWalkMode GetWalkMode() const;
	
	//////////////////////
	/// 动画相关

	/** 获取世界空间中的移动方向(单位向量)，通常Z = 0.0f */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|MovementAgent")
	virtual FVector GetWorldMovementIntent() const { return FVector::ZeroVector; }
	/** 获取本地空间中的移动方向(单位向量)，通常Z = 0.0f */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|MovementAgent")
	virtual FVector GetLocalMovementIntent() const { return FVector::ZeroVector; }

	/** 绘制调试移动方向箭头 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|MovementAgent")
	virtual void DrawDebugMovementIntent() const;
};
