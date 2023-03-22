// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SLCharacterMovementComponent.generated.h"

/**
 * 稍微修改功能的角色移动组件。
 * - 主要意图是使角色在根运动时不会把其他角色弹飞，且不会从目标角色身边滑过去。
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SEKIROLIKE_API USLCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	virtual float SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact) override;
};
