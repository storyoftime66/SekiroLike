// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DidItHitActorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDidItHitDelegate, FHitResult, LastHit);


class UStaticMeshComponent;

/**
 * 攻击判定检测组件。
 * - 参考DidItHit插件的思路实现，DidItHit的简化版。
 *		1. DidItHitActorComponent需要用静态网格体初始化后才能进行检测。
 *		2. DidItHitActorComponent会获取静态网格体中所有的插槽作为检测点。
 * - DitItHit插件商城链接：https://www.unrealengine.com/marketplace/zh-CN/product/did-it-hit-plugin
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName="AC_DidItHit"))
class DIDITHIT_API UDidItHitActorComponent : public UActorComponent
{
	GENERATED_BODY()

	/** 用于攻击检测的静态网格体组件，通常是近战武器、子弹等。*/
	TWeakObjectPtr<UStaticMeshComponent> StaticMeshComp;

#pragma region INTERNAL
	/** 检测使用到的所有插槽名称 */
	TArray<FName> SocketNames;

	/** 所有插槽上一刻的位置 */
	TMap<FName, FVector> LastKnownSocketLocation;

	/** 当前是否开启检测 */
	bool bCanTrace = false;

	/** 命中的Actor */
	UPROPERTY()
	TArray<AActor*> HitActors;

	/** 初始忽略的Actor */
	UPROPERTY()
	TArray<AActor*> IgnoredActors;

	/** 检测时忽略的Actor，避免重复检测到同一Actor */
	UPROPERTY()
	TArray<AActor*> TempIgnoredActors;
#pragma endregion

public:
#pragma region TRACE_CONFIG
	//////////////////
	/// 检测模式
	//////////////////

	/** 在不同时刻检测同一的插槽位置。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Did It Hit")
	bool bTraceSamePointAtDifferentTime = true;
	
	/** 在同一刻检测不同的插槽位置。小心，复杂度 O(n^2)，n为网格体插槽数量。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Did It Hit")
	bool bTraceDifferentPointAtSameTime = false;
	
	/** 在不同时刻检测不同的插槽位置。小心，复杂度 O(n^2)，n为网格体插槽数量。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Did It Hit")
	bool bTraceDifferentPointAtDifferentTime = false;

	//////////////////
	/// 检测参数
	//////////////////

	/** 检测通道 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Did It Hit")
	TEnumAsByte<ETraceTypeQuery> TraceChannel;
	
	/** 是否检测复杂碰撞 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Did It Hit")
	bool bTraceComplex = true;
	
	/** 是否画出调试线条 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Did It Hit")
	bool bDrawDebug = false;
#pragma endregion

protected:
	/** 执行线条检测 */
	void LineTraceAndHandleHit(const FVector& Start, const FVector& End);
	
	/** 更新上一刻插槽位置 */
	void UpdateLastKnownSocketLoc();

public:
	UDidItHitActorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 命中新的Actor事件 */
	UPROPERTY(BlueprintAssignable, Category="Did It Hit")
	FDidItHitDelegate OnHitActor;

	/** 设置检测使用的静态网格体 */
	UFUNCTION(BlueprintCallable, Category="Did It Hit")
	void SetupDitItHitComp(UStaticMeshComponent* NewStaticMeshComp);

	/** 添加需要忽略的Actor */
	UFUNCTION(BlueprintCallable, Category="Did It Hit")
	void AddIgnoredActor(AActor* IgnoredActor);

	/** 添加需要忽略的Actors */
	UFUNCTION(BlueprintCallable, Category="Did It Hit")
	void AddIgnoredActors(TArray<AActor*> InIgnoredActor);

	/** 切换激活检测 */
	UFUNCTION(BlueprintCallable, Category="Did It Hit")
	void ToggleTraceCheck(bool NewCanTrace);

	/** 单次检测结束后可调用，获取当次检测到的所有Actor */
	UFUNCTION(BlueprintCallable, Category="Did It Hit")
	TArray<AActor*> GetHitActors() const;
};
