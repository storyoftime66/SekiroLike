// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/SLCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Characters/BaseCharacter.h"

#define COS140 FMath::Cos(PI * 140.0f / 180.0f)
#define TRACE_DISTANCE 10000.0f

const FSLCharacterGroundInfo& USLCharacterMovementComponent::GetGroundInfo() {
	if (!CharacterOwner || CachedGroundInfo.LastUpdateFrame == GFrameCounter) {
		return CachedGroundInfo;
	}

	if (MovementMode == EMovementMode::MOVE_Walking) {
		CachedGroundInfo.HitResult = CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.0f;
	} else {
		// Copy from LyraCharacterMovementComponent
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);

		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
		const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);
		const FVector TraceStart(GetActorLocation());
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - TRACE_DISTANCE - CapsuleHalfHeight));

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LyraCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(QueryParams, ResponseParam);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParam);

		CachedGroundInfo.HitResult = HitResult;
		CachedGroundInfo.GroundDistance = TRACE_DISTANCE;

		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.0f;
		}
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}

	CachedGroundInfo.LastUpdateFrame = GFrameCounter;
	return CachedGroundInfo;
}

float USLCharacterMovementComponent::SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact)
{
	// 主要意图是使角色在根运动时不会把其他角色弹飞，且不会从目标角色身边滑过去。
	if (Hit.IsValidBlockingHit())
	{
		if (auto BlockActor = Cast<ABaseCharacter>(Hit.GetActor()))
		{
			// 正对敌人，且继续往前移动时不发生滑动
			if (Delta.CosineAngle2D(Normal) < COS140)
			{
				return 0.0f;
			}
		}
	}

	return Super::SlideAlongSurface(Delta, Time, Normal, Hit, bHandleImpact);
}
