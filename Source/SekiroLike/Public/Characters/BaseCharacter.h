// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UGameplayAbility;
class UCharAttributeSet;
class UInputAction;

// USTRUCT()
// struct FBaseInputSetting : public FTableRowBase
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, Category="基本操作")
// 	UInputAction* IA_Move;
// 	UPROPERTY()
// 	UInputAction* IA_Look;
// 	UPROPERTY(EditAnywhere, Category="基本操作")
// 	UInputAction* IA_Focus;
// 	UPROPERTY(EditAnywhere, Category="基本操作")
// 	UInputAction* IA_Attack;
// 	UPROPERTY(EditAnywhere, Category="基本操作")
// 	UInputAction* IA_Block;
// };

UCLASS()
class SEKIROLIKE_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category="SekiroLike|Abilities")
	UAbilitySystemComponent* ASC;
	UPROPERTY()
	UCharAttributeSet* CharAS;
	/** 初始技能组 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SekiroLike|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> InitialAbilities;

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:
	virtual void Tick(float DeltaTime) override;

	//~ IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }
};
