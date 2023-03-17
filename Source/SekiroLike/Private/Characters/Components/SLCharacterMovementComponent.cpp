// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/SLCharacterMovementComponent.h"

#include "Characters/BaseCharacter.h"

#define COS140 FMath::Cos(PI * 140.0f / 180.0f)

float USLCharacterMovementComponent::SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact)
{
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
