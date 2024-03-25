// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SLCharacterMovementComponent.generated.h"


USTRUCT(BlueprintType)
struct FSLCharacterGroundInfo {
	GENERATED_BODY()

	FSLCharacterGroundInfo():
		LastUpdateFrame(0),
		GroundDistance(0.0)
	{}

	uint64 LastUpdateFrame;

	/** 对地面的射线检测结果 */
	UPROPERTY(BlueprintReadOnly)
	FHitResult HitResult;

	/** 离地距离 */
	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

/**
 * 稍微修改功能的角色移动组件。
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SEKIROLIKE_API USLCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	/** 返回当前的地面信息 */
	UFUNCTION(BlueprintPure, Category = "SekiroLike|Movement")
	const FSLCharacterGroundInfo& GetGroundInfo();

protected:
	virtual float SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact) override;
	
	FSLCharacterGroundInfo CachedGroundInfo;
};
