// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/PlayerFocusComp.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Characters/AbilityAvatarInterface.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UPlayerFocusComp::UPlayerFocusComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UPlayerFocusComp::IsFocused() const
{
	return FocusedCharacter.IsValid();
}

ACharacter* UPlayerFocusComp::GetFocusedCharacter() const
{
	return FocusedCharacter.Get();
}

void UPlayerFocusComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bInitialized)
	{
		if (FocusedCharacter.IsValid())
		{
			AActor* CompOwner = GetOwner();
			// 旋转控制器（控制器带动摄像机）
			if (CompOwner)
			{
				// 旋转角色朝向锁定目标
				const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(CompOwner->GetActorLocation(), FocusedCharacter->GetActorLocation());
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
				if (FVector::DistSquared(CompOwner->GetActorLocation(), FocusedCharacter->GetActorLocation()) > MaxDistanceSquare)
				{
					SetFocusedCharacter(nullptr);
					return;
				}
			}

			// 角色死亡解除锁定
			if (auto AbilityAvatar = Cast<IAbilityAvatarInterface>(FocusedCharacter))
			{
				if (AbilityAvatar->IsDead())
				{
					SetFocusedCharacter(nullptr);
					return;
				}
			}
		}
		else
		{
			FocusedCharacter.Reset();
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
}

void UPlayerFocusComp::Focus()
{
	check(CompOwnerPC.IsValid());
	if (!bInitialized)
	{
		return;
	}

	if (FocusedCharacter.IsValid())
	{
		SetFocusedCharacter(nullptr);
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
			SetFocusedCharacter(Cast<ACharacter>(OutHit.GetActor()));
		}
	}
}

void UPlayerFocusComp::SwitchTarget(const FInputActionValue& Value)
{
	check(GetOwner() && GetWorld());

	if (!IsFocused())
	{
		return;
	}

	if (Value.GetMagnitude() == 0.0f)
	{
		return;
	}

	// Note: 负表示逆时针（左边），正表示顺时针（右边）
	const float Direction = FMath::Sign(Value.GetMagnitude());
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
				ACharacter* NewFocusedCharacter = Cast<ACharacter>(Hit.GetActor());
				if (NewFocusedCharacter && NewFocusedCharacter != FocusedCharacter)
				{
					// 判断找到的新目标是否和旋转方向一致
					FVector V1 = NewFocusedCharacter->GetActorLocation() - GetOwner()->GetActorLocation();
					V1.Z = 0.0f;
					FVector V2 = FocusedCharacter->GetActorLocation() - GetOwner()->GetActorLocation();
					V2.Z = 0.0f;
					// Note: 由于虚幻用左手系，所以用的左手螺旋定则
					if (FMath::Sign(FVector::CrossProduct(V2, V1).Z) == Direction)
					{
						SetFocusedCharacter(NewFocusedCharacter);
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

void UPlayerFocusComp::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("UPlayerFocusComp::BeginPlay() called."))

	// 初始化
	const ACharacter* CompOwner = Cast<ACharacter>(GetOwner());
	if (IsValid(CompOwner))
	{
		CompOwnerPC = CompOwner->GetController<APlayerController>();
		CharacterMovement = CompOwner->GetCharacterMovement();
		bInitialized = CompOwnerPC.IsValid() && CharacterMovement.IsValid();
	}

	if (!bInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("PC or CharacterMovement is invalid, the focuscomp is not initialized."))
	}
}

void UPlayerFocusComp::SetFocusedCharacter(ACharacter* NewCharacter)
{
	if (IsValid(ReticleClass))
	{
		// 移除原来的reticle并添加新的reticle
		if (FocusedCharacter.IsValid() && IsValid(ReticleWidgetComp))
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

	FocusedCharacter = NewCharacter;
}
