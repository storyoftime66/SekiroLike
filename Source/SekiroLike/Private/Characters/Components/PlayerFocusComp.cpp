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

void UPlayerFocusComp::InitializeFocusComp(APlayerController* InPC, UCharacterMovementComponent* InCharacterMovement)
{
	if (!IsValid(InPC) or !IsValid(InCharacterMovement))
	{
		UE_LOG(LogTemp, Warning, TEXT("PC or CharacterMovement is invalid, the focuscomp is not initialized."))
		return;
	}

	CompOwnerPC = InPC;
	DefaultControlInputScale = InPC->InputPitchScale;
	CharacterMovement = InCharacterMovement;

	// 获取CompOwner
	UObject* CurrentObject = GetOuter();
	while (CurrentObject)
	{
		AActor* Actor = Cast<AActor>(CurrentObject);
		if (Actor)
		{
			CompOwner = Actor;
			break;
		}
		CurrentObject = CurrentObject->GetOuter();
	}

	QueryParams = FCollisionQueryParams(FName("PlayerFocusAction"), SCENE_QUERY_STAT_ONLY(Focus), true);
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.bReturnFaceIndex = false;
	QueryParams.AddIgnoredActor(GetOwner());

	bInitialized = IsValid(CompOwnerPC) and IsValid(CharacterMovement) and IsValid(CompOwner);
}

bool UPlayerFocusComp::IsFocused() const
{
	return IsValid(FocusedCharacter);
}

ACharacter* UPlayerFocusComp::GetFocusedCharacter() const
{
	return IsValid(FocusedCharacter) ? FocusedCharacter : nullptr;
}

void UPlayerFocusComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bInitialized)
	{
		if (IsValid(FocusedCharacter))
		{
			check(CompOwner);
			check(CompOwnerPC);
			check(CharacterMovement);

			// 旋转控制器（控制器带动摄像机）
			if (CompOwner)
			{
				const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(CompOwner->GetActorLocation(), FocusedCharacter->GetActorLocation());
				const FRotator ActorTargetRot(0.0f, LookAtRot.Yaw, 0.0f);
				CompOwner->SetActorRotation(FMath::RInterpTo(CompOwner->GetActorRotation(), ActorTargetRot, DeltaTime, 10.0f));
				if (CompOwnerPC)
				{
					CompOwnerPC->InputPitchScale = CompOwnerPC->InputYawScale = 0.0f;
					const FRotator ControlTargetRot(-15.0f, LookAtRot.Yaw, 0.0f);
					CompOwnerPC->SetControlRotation(FMath::RInterpTo(CompOwnerPC->GetControlRotation(), ControlTargetRot, DeltaTime, 4.0f));
				}
			}

			// 修改移动组件设置
			if (CharacterMovement)
			{
				CharacterMovement->bOrientRotationToMovement = false;
			}

			// 距离过远则解除锁定
			if (FVector::DistSquared(CompOwner->GetActorLocation(), FocusedCharacter->GetActorLocation()) > MaxDistanceSquare)
			{
				SetFocusedCharacter(nullptr);
				return;
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
			FocusedCharacter = nullptr;
			if (CharacterMovement)
			{
				CharacterMovement->bOrientRotationToMovement = true;
			}
			if (CompOwnerPC)
			{
				CompOwnerPC->InputPitchScale = CompOwnerPC->InputYawScale = DefaultControlInputScale;
			}
		}
	}
}

void UPlayerFocusComp::Focus()
{
	check(CompOwnerPC);
	if (!bInitialized)
	{
		return;
	}

	if (IsValid(FocusedCharacter))
	{
		SetFocusedCharacter(nullptr);
	}
	else
	{
		// 检测并获取可以锁定的敌人
		check(CompOwner and CompOwner->GetWorld());

		FVector CamLoc;
		FRotator CamRot;
		CompOwnerPC->GetPlayerViewPoint(CamLoc, CamRot);
		CamRot.Pitch = 0.0f;

		FVector Start = CompOwner->GetActorLocation();
		FVector End = Start + CamRot.Vector() * SweepDistance;

		FHitResult OutHit;
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
	check(CompOwner and CompOwner->GetWorld());

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
	FVector Start = CompOwner->GetActorLocation();
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
				if (NewFocusedCharacter and NewFocusedCharacter != FocusedCharacter)
				{
					// 判断找到的新目标是否和旋转方向一致
					FVector V1 = NewFocusedCharacter->GetActorLocation() - CompOwner->GetActorLocation();
					V1.Z = 0.0f;
					FVector V2 = FocusedCharacter->GetActorLocation() - CompOwner->GetActorLocation();
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
	check(CompOwner and CompOwner->GetWorld());
	const bool bHit = GetWorld()->SweepMultiByObjectType(
		OutHits, Start, Start + DirVector, FQuat::Identity, FCollisionObjectQueryParams(FocusObject.GetValue()),
		FCollisionShape::MakeSphere(400.0f), QueryParams);
	// DrawDebugCylinder(CompOwner->GetWorld(), Start, Start + DirVector, 400.0f, 12, FColor::Red, true, 10.0f, 0, 4.0f);
	return bHit;
}

void UPlayerFocusComp::SetFocusedCharacter(ACharacter* NewCharacter)
{
	if (IsValid(ReticleClass))
	{
		// 移除原来的reticle并添加新的reticle
		if (IsValid(FocusedCharacter) and IsValid(ReticleWidgetComp))
		{
			ReticleWidgetComp->DestroyComponent();
			ReticleWidgetComp = nullptr;
		}
		if (IsValid(NewCharacter))
		{
			ReticleWidgetComp = NewObject<UWidgetComponent>(NewCharacter, UWidgetComponent::StaticClass());
			ReticleWidgetComp->RegisterComponent();
			ReticleWidgetComp->AttachToComponent(NewCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			auto ReticleWidget = CreateWidget(CompOwnerPC, ReticleClass);
			ReticleWidgetComp->SetWidget(ReticleWidget);
			ReticleWidgetComp->SetCollisionProfileName("NoCollision");
			ReticleWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
			ReticleWidgetComp->SetDrawSize(FVector2D(128.0f, 128.0f));
		}
	}

	FocusedCharacter = NewCharacter;
}
