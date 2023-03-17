// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Shared/GameplayAbilityTargetActor_DidItHit.h"
#include "Abilities/GameplayAbility.h"
#include "DidItHit/Public/DidItHitActorComponent.h"
#include "GameFramework/Character.h"


// Sets default values
AGameplayAbilityTargetActor_DidItHit::AGameplayAbilityTargetActor_DidItHit()
{
	// 本身不需要Tick，仅DidItHit组件Tick
	PrimaryActorTick.bCanEverTick = false;
	ShouldProduceTargetDataOnServer = true;
	bDestroyOnConfirmation = false;

	DidItHitComp = CreateDefaultSubobject<UDidItHitActorComponent>(TEXT("DidItHitComp"));
}

void AGameplayAbilityTargetActor_DidItHit::BeginPlay()
{
	Super::BeginPlay();
}

FGameplayAbilityTargetDataHandle AGameplayAbilityTargetActor_DidItHit::MakeTargetData(FHitResult& HitResult)
{
	return StartLocation.MakeTargetDataHandleFromHitResult(OwningAbility, HitResult);
}

void AGameplayAbilityTargetActor_DidItHit::OnHitActorHandler(FHitResult HitResult)
{
	if (IsConfirmTargetingAllowed())
	{
		TargetDataReadyDelegate.Broadcast(MakeTargetData(HitResult));
	}
}

void AGameplayAbilityTargetActor_DidItHit::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	bIsTargeting = true;
	DidItHitComp->ToggleTraceCheck(true);
}

void AGameplayAbilityTargetActor_DidItHit::ConfirmTargetingAndContinue()
{
	DidItHitComp->ToggleTraceCheck(false);
	bIsTargeting = false;
}

bool AGameplayAbilityTargetActor_DidItHit::IsConfirmTargetingAllowed()
{
	return bIsTargeting;
}

AGameplayAbilityTargetActor_DidItHit* AGameplayAbilityTargetActor_DidItHit::MakeTargetActor_DidItHit(ACharacter* AbilityAvatar)
{
	if (!IsValid(AbilityAvatar))
	{
		return nullptr;
	}

	check(AbilityAvatar->GetWorld());

	auto TargetActor = AbilityAvatar->GetWorld()->SpawnActor<AGameplayAbilityTargetActor_DidItHit>(AbilityAvatar->GetActorLocation(), FRotator::ZeroRotator);
	if (TargetActor)
	{
		TargetActor->DidItHitComp->SetupDitItHitComp(
			Cast<UStaticMeshComponent>(AbilityAvatar->GetMesh()->GetChildComponent(0)));
		TargetActor->DidItHitComp->OnHitActor.AddDynamic(TargetActor, &AGameplayAbilityTargetActor_DidItHit::OnHitActorHandler);
	}
	return TargetActor;
}
