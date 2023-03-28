// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"

#include "Abilities/GameplayAbilityBase_PassiveAbility.h"
#include "Abilities/SLAbilityTypes.h"
#include "Characters/CharAttributeSet.h"
#include "Characters/Components/SLCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer.SetDefaultSubobjectClass<USLCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Actor
	PrimaryActorTick.bCanEverTick = true;

	// Pawn
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Character
	if (GetMesh())
	{
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetCollisionProfileName(FName("SLCharacterMesh"));
	}
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionProfileName(FName("SLCharacterCapsule"));
	}

	CharAS = CreateDefaultSubobject<UCharAttributeSet>(TEXT("CharAS"));
	ASC = CreateDefaultSubobject<USLAbilitySystemComponent>(TEXT("ASC"));
}

void ABaseCharacter::ReactToHit(FName BoneName, FVector HitImpulse)
{
	if (!GetMesh())
	{
		return;
	}

	HitBoneName = BoneName;
	GetMesh()->SetAllBodiesBelowSimulatePhysics(BoneName, true, true);
	GetMesh()->AddImpulse(HitImpulse, BoneName);
	ReactToHitTimeline.PlayFromStart();
}

void ABaseCharacter::NotifyDied(AActor* InInstigator)
{
	const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ABaseCharacter::Died, InInstigator);
	GetWorldTimerManager().SetTimerForNextTick(TimerDelegate);
}

void ABaseCharacter::ReactToHitFinished()
{
	if (GetMesh())
	{
		GetMesh()->SetAllBodiesSimulatePhysics(false);
	}
}

void ABaseCharacter::ReactToHitTimelineProgress(float Value)
{
	if (GetMesh())
	{
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(HitBoneName, Value);
	}
}


void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 初始化技能系统组件
	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);

		// 赋予初始效果
		for (const auto EffectClass : InitialEffects)
		{
			if (EffectClass)
			{
				const UGameplayEffect* Effect = EffectClass->GetDefaultObject<UGameplayEffect>();
				ASC->ApplyGameplayEffectToSelf(Effect, 1.0f, ASC->MakeEffectContext());
			}
		}

		// 赋予初始技能
		for (const auto Ability : InitialAbilities)
		{
			auto SpecHandle = ASC->GiveAbility(FGameplayAbilitySpec(Ability));

			TSubclassOf<UGameplayAbilityBase_PassiveAbility> PassiveAbility;
			PassiveAbility = Ability;
			if (PassiveAbility && PassiveAbility.GetDefaultObject()->bActivateOnce)
			{
				ASC->TryActivateAbility(SpecHandle);
				PersistentSpecHandles.Add(SpecHandle);
			}
		}
	}

	// 初始化物理动画设置
	if (GetMesh() && ReactToHitCurve)
	{
		ReactToHitTimeline = FTimeline();
		FOnTimelineFloat OnReactToHitTimeline;
		OnReactToHitTimeline.BindUFunction(this, FName("ReactToHitTimelineProgress"));
		ReactToHitTimeline.AddInterpFloat(ReactToHitCurve, OnReactToHitTimeline);

		OnReactToHitFinished.BindUFunction(this, FName("ReactToHitFinished"));
		ReactToHitTimeline.SetTimelineFinishedFunc(OnReactToHitFinished);
	}
}

void ABaseCharacter::Died_Implementation(AActor* InInstigator)
{
	if (IsValid(GetCapsuleComponent()))
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}

	if (IsValid(GetMesh()))
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("root"), true);
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("root"), 1.0f);
	}

	if (IsValid(ASC))
	{
		ASC->CancelAbilities();
		ASC->ClearAllAbilities();
	}

	IsCharacterDead = true;
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static FGameplayTagContainer ActiveAbilityTags = FGameplayTagContainer(FGameplayTag::RequestGameplayTag("SekiroLike.Ability.ActiveAbility"));

	ReactToHitTimeline.TickTimeline(DeltaTime);

	// TODO: 状态有Bug，会导致ASC残留ActiveAbility的 BlockedTag，但是还找不到原因
	if (IsValid(ASC) && ASC->AreAbilityTagsBlocked(ActiveAbilityTags))
	{
		TimeAccumulation += DeltaTime;
		if (TimeAccumulation > 2.0f)
		{
			TimeAccumulation = 0.0f;
			ASC->UnBlockAbilitiesWithTags(ActiveAbilityTags);
		}
	}
	else
	{
		TimeAccumulation = 0.0f;
	}
}

float ABaseCharacter::GetAheadFoot() const
{
	static FName LeftFootBoneName("Foot_L");
	static FName RightFootBoneName("Foot_R");

	if (GetMesh())
	{
		const FVector LeftFootLoc = GetMesh()->GetBoneLocation(LeftFootBoneName, EBoneSpaces::ComponentSpace);
		const FVector RightFootLoc = GetMesh()->GetBoneLocation(RightFootBoneName, EBoneSpaces::ComponentSpace);
		// Note: 除数大概选的
		return FMath::Clamp((RightFootLoc.X - LeftFootLoc.X) / 20.0f, -1.0f, 1.0f);
	}

	return 0.0f;
}

void ABaseCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (const auto TeamAgent = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		return TeamAgent->SetGenericTeamId(NewTeamID);
	}
}

FGenericTeamId ABaseCharacter::GetGenericTeamId() const
{
	if (const auto TeamAgent = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		return TeamAgent->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

ETeamAttitude::Type ABaseCharacter::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const auto TeamAgent = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		return TeamAgent->GetTeamAttitudeTowards(Other);
	}
	return ETeamAttitude::Neutral;
}

void ABaseCharacter::SetWalkMode(EWalkMode NewWalkMode)
{
	WalkMode = NewWalkMode;
	if (GetCharacterMovement())
	{
		float NewSpeed = RunSpeed;
		switch (NewWalkMode)
		{
		case Walk:
			NewSpeed = WalkSpeed;
			break;
		case None:
			NewSpeed = 0.0f;
			break;
		default:
			break;
		}
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	}
}

EWalkMode ABaseCharacter::GetWalkMode() const
{
	return WalkMode;
}

void ABaseCharacter::AddAbilityStage(const FGameplayTag& AbilityStageTag)
{
	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->AddLooseGameplayTag(AbilityStageTag);
	}
}

void ABaseCharacter::ClearAbilityStage()
{
	SLAbilityTypes::ClearAllStageTags(this);
}

FVector ABaseCharacter::GetWorldMovementIntent() const
{
	return GetVelocity().GetSafeNormal2D();
}

FVector ABaseCharacter::GetLocalMovementIntent() const
{
	const FVector RawLocalMovementIntent = (GetActorRotation() * -1.0f).RotateVector(GetWorldMovementIntent());
	return RawLocalMovementIntent.GetSafeNormal2D();
}
