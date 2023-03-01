// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Characters/CharAttributeSet.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CharAS = CreateDefaultSubobject<UCharAttributeSet>(TEXT("CharAS"));
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);
			for(const auto Ability : InitialAbilities)
			{
				ASC->GiveAbility(FGameplayAbilitySpec(Ability));
			}
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

