// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
struct FInputActionValue;
class UInputMappingContext;
UCLASS()
class SEKIROLIKE_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	/** 跟随的弹簧臂 */
	UPROPERTY(EditAnywhere, Category="Dev")
	USpringArmComponent* FollowSpringArm;
	/** 跟随的相机 */
	UPROPERTY(EditAnywhere, Category="Dev")
	UCameraComponent* FollowCamera;

protected:
	UPROPERTY()
	ACharacter* FocusedEnemy;
	FVector2D MovementInput;
	////////////////////////////////////////
	/// 可用的输入动作
	UPROPERTY(EditAnywhere, Category="SekiroLike|Input")
	UInputMappingContext* DefaultMC;
	UPROPERTY()
	UInputAction* IA_Move;
	UPROPERTY()
	UInputAction* IA_Look;
	UPROPERTY()
	UInputAction* IA_Attack;
	UPROPERTY()
	UInputAction* IA_Block;
	UPROPERTY()
	UInputAction* IA_Focus;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

#pragma region HELPER_FUNCTIONS
	/** 应用新的输入映射 */
	void ApplyInputMappingContext(const UInputMappingContext* NewMappingContext, int32 Priority) const;
#pragma endregion

#pragma region PLAYER_INPUT_HANDLER
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack();
	void Block();
	void Focus();
#pragma endregion

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void UnPossessed() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
