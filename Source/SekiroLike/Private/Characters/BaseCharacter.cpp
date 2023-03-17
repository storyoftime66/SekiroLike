// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"

#include "AssetTypeCategories.h"
#include "Abilities/GameplayAbilityBase_PassiveAbility.h"
#include "Abilities/SLAbilityTypes.h"
#include "Characters/CharAttributeSet.h"
#include "Characters/Components/SLCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer.SetDefaultSubobjectClass<USLCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	CharAS = CreateDefaultSubobject<UCharAttributeSet>(TEXT("CharAS"));
	ASC = CreateDefaultSubobject<USLAbilitySystemComponent>(TEXT("ASC"));

	if (GetMesh())
	{
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetCollisionObjectType(ECC_Pawn);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		// Note: 需要的话则手动将武器检测设为Overlap
	}
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

void ABaseCharacter::ReactToHitFinished()
{
	if (GetMesh())
	{
		GetMesh()->SetAllBodiesSimulatePhysics(false);
		UE_LOG(LogTemp, Display, TEXT("ReactToHitFinished"));
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

	// 设置技能系统组件
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
			if (PassiveAbility and PassiveAbility.GetDefaultObject()->bActivateOnce)
			{
				ASC->TryActivateAbility(SpecHandle);
				PersistentSpecHandles.Add(SpecHandle);
			}
		}
	}

	// 设置物理动画
	if (GetMesh() and ReactToHitCurve)
	{
		ReactToHitTimeline = FTimeline();
		FOnTimelineFloat OnReactToHitTimeline;
		OnReactToHitTimeline.BindUFunction(this, FName("ReactToHitTimelineProgress"));
		ReactToHitTimeline.AddInterpFloat(ReactToHitCurve, OnReactToHitTimeline);

		OnReactToHitFinished.BindUFunction(this, FName("ReactToHitFinished"));
		ReactToHitTimeline.SetTimelineFinishedFunc(OnReactToHitFinished);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ReactToHitTimeline.TickTimeline(DeltaTime);
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
		default:
			// Note: 需不需要设为0？
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
