// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/PlayerFocusComp.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Characters/AbilityAvatarInterface.h"
#include "Components/WidgetComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UPlayerFocusComp::UPlayerFocusComp(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UPlayerFocusComp::IsTargeting() const {
	return TargetCharacter.IsValid();
}

ACharacter* UPlayerFocusComp::GetTargetCharacter() const
{
	return TargetCharacter.Get();
}

void UPlayerFocusComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsTargeting())
	{
		AActor* CompOwner = GetOwner();
		// 旋转控制器（控制器带动摄像机）
		if (CompOwner)
		{
			// 旋转角色朝向锁定目标
			const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(CompOwner->GetActorLocation(), TargetCharacter->GetActorLocation());
			const FRotator ActorTargetRot(0.0f, LookAtRot.Yaw, 0.0f);
			CompOwner->SetActorRotation(FMath::RInterpTo(CompOwner->GetActorRotation(), ActorTargetRot, DeltaTime, 10.0f));

			if (CompOwnerPC.IsValid())
			{
				if (!bIgnoreLookInput)
				{
					CompOwnerPC->SetIgnoreLookInput(true);
					bIgnoreLookInput = true;
				}
					
				const FRotator ControlTargetRot(-15.0f, LookAtRot.Yaw, 0.0f);
				CompOwnerPC->SetControlRotation(FMath::RInterpTo(CompOwnerPC->GetControlRotation(), ControlTargetRot, DeltaTime, 4.0f));
			}
		}

		// 修改移动组件设置
		if (CharacterMovement.IsValid())
		{
			CharacterMovement->bOrientRotationToMovement = false;
		}

		// 距离过远则解除锁定
		if (CompOwner)
		{
			if (FVector::DistSquared(CompOwner->GetActorLocation(), TargetCharacter->GetActorLocation()) > MaxDistanceSquare)
			{
				SetTargetCharacter(nullptr);
				return;
			}
		}

		// 角色死亡解除锁定
		if (auto AbilityAvatar = Cast<IAbilityAvatarInterface>(TargetCharacter))
		{
			if (AbilityAvatar->IsDead())
			{
				SetTargetCharacter(nullptr);
				return;
			}
		}
	}
	else
	{
		TargetCharacter.Reset();
		if (CharacterMovement.IsValid())
		{
			CharacterMovement->bOrientRotationToMovement = true;
		}
		if (CompOwnerPC.IsValid() && bIgnoreLookInput)
		{
			CompOwnerPC->SetIgnoreLookInput(false);
			bIgnoreLookInput = false;
		}
	}
}

void UPlayerFocusComp::ToggleFocus() {
	if (!CompOwnerPC.IsValid()) {
		const ACharacter* CompOwner = CastChecked<ACharacter>(GetOwner());
		CompOwnerPC = CompOwner->GetController<APlayerController>();
		check(CompOwnerPC.IsValid());
	}
	if (!CharacterMovement.IsValid()) {
		const ACharacter* CompOwner = CastChecked<ACharacter>(GetOwner());
		CharacterMovement = CompOwner->GetCharacterMovement();
		check(CharacterMovement.IsValid());
	}
	SetComponentTickEnabled(true);

	if (IsTargeting())
	{
		SetTargetCharacter(nullptr);
	}
	else
	{
		// 检测并获取可以锁定的敌人
		check(GetOwner() && GetWorld());

		FVector CamLoc;
		FRotator CamRot;
		CompOwnerPC->GetPlayerViewPoint(CamLoc, CamRot);
		CamRot.Pitch = 0.0f;

		FVector Start = GetOwner()->GetActorLocation();
		FVector End = Start + CamRot.Vector() * SweepDistance;

		FHitResult OutHit;
		FCollisionQueryParams QueryParams;
		QueryParams = FCollisionQueryParams(FName("PlayerFocusAction"), SCENE_QUERY_STAT_ONLY(Focus), true);
		QueryParams.bReturnPhysicalMaterial = false;
		QueryParams.bReturnFaceIndex = false;
		QueryParams.AddIgnoredActor(GetOwner());
		const bool bHit = GetWorld()->SweepSingleByObjectType(
			OutHit, Start, End, FQuat::Identity, FCollisionObjectQueryParams(FocusObject.GetValue()),
			FCollisionShape::MakeSphere(400.0f), QueryParams);
		if (bHit)
		{
			SetTargetCharacter(Cast<ACharacter>(OutHit.GetActor()));
		}
	}
}

void UPlayerFocusComp::SwitchFocusTarget(const FInputActionValue& Value)
{
	check(GetOwner() && GetWorld());

	if (!IsTargeting())
	{
		return;
	}

	if (Value.GetMagnitude() == 0.0f)
	{
		return;
	}

	// Note:正表示逆时针（左边），负表示顺时针（右边）
	const float Direction = -FMath::Sign(Value.GetMagnitude());
	FVector CamLoc;
	FRotator CamRot;
	CompOwnerPC->GetPlayerViewPoint(CamLoc, CamRot);
	CamRot.Pitch = 0.0f;
	FVector Start = GetOwner()->GetActorLocation();
	FVector OriginalDirVector = CamRot.Vector() * SweepDistance;
	for (float SweepAngle = 0.0f; SweepAngle <= SwitchSweepAngle; SweepAngle += 15.0f)
	{
		FVector SweepDirVector = FRotator(0.0f, SweepAngle * Direction, 0.0f).RotateVector(OriginalDirVector);
		TArray<FHitResult> OutHits;
		bool bHit = SphereSweepMulti(OutHits, Start, SweepDirVector);
		if (bHit)
		{
			for (auto& Hit : OutHits)
			{
				ACharacter* NewTargetCharacter = Cast<ACharacter>(Hit.GetActor());
				if (NewTargetCharacter && NewTargetCharacter != TargetCharacter)
				{
					// 判断找到的新目标是否和旋转方向一致
					FVector V1 = NewTargetCharacter->GetActorLocation() - GetOwner()->GetActorLocation();
					V1.Z = 0.0f;
					FVector V2 = TargetCharacter->GetActorLocation() - GetOwner()->GetActorLocation();
					V2.Z = 0.0f;
					// Note: 由于虚幻用左手系，所以用的左手螺旋定则
					if (FMath::Sign(FVector::CrossProduct(V2, V1).Z) == Direction)
					{
						SetTargetCharacter(NewTargetCharacter);
						return;
					}
				}
			}
		}
	}
}

bool UPlayerFocusComp::SphereSweepMulti(TArray<FHitResult>& OutHits, FVector Start, FVector DirVector) const
{
	check(GetOwner() && GetWorld());

	FCollisionQueryParams QueryParams;
	QueryParams = FCollisionQueryParams(FName("PlayerFocusAction"), SCENE_QUERY_STAT_ONLY(Focus), true);
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.bReturnFaceIndex = false;
	QueryParams.AddIgnoredActor(GetOwner());
	const bool bHit = GetWorld()->SweepMultiByObjectType(
		OutHits, Start, Start + DirVector, FQuat::Identity, FCollisionObjectQueryParams(FocusObject.GetValue()),
		FCollisionShape::MakeSphere(400.0f), QueryParams);
	return bHit;
}

void UPlayerFocusComp::SetTargetCharacter(ACharacter* NewCharacter)
{
	if (IsValid(ReticleClass))
	{
		// 移除原来的reticle并添加新的reticle
		if (TargetCharacter.IsValid() && IsValid(ReticleWidgetComp))
		{
			ReticleWidgetComp->DestroyComponent();
			ReticleWidgetComp = nullptr;
		}
		if (IsValid(NewCharacter))
		{
			ReticleWidgetComp = NewObject<UWidgetComponent>(NewCharacter, UWidgetComponent::StaticClass());
			ReticleWidgetComp->RegisterComponent();
			ReticleWidgetComp->AttachToComponent(NewCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			auto ReticleWidget = CreateWidget(CompOwnerPC.Get(), ReticleClass);
			ReticleWidgetComp->SetWidget(ReticleWidget);
			ReticleWidgetComp->SetCollisionProfileName("NoCollision");
			ReticleWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
			ReticleWidgetComp->SetDrawSize(FVector2D(128.0f, 128.0f));
		}
	}

	TargetCharacter = NewCharacter;
}

void UPlayerFocusComp::BindInput(UEnhancedInputComponent* EnhancedInputComp) {
	EnhancedInputComp->BindAction(IA_ToggleFocus, ETriggerEvent::Started, this, &UPlayerFocusComp::ToggleFocus);
	EnhancedInputComp->BindAction(IA_SwitchFocusTarget, ETriggerEvent::Started, this, &UPlayerFocusComp::SwitchFocusTarget);
}