// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Abilities/GameplayAbilityBase_ActiveAbility.h"
#include "Abilities/SLAbilityTypes.h"
#include "Camera/CameraComponent.h"
#include "Characters/Components/PlayerFocusComp.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Pawn
	bUseControllerRotationYaw = false;

	// 跟随弹簧臂和摄像机组件
	FollowSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FollowSpringArm"));
	FollowSpringArm->SetupAttachment(RootComponent);
	FollowSpringArm->TargetArmLength = 400.0f;
	FollowSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	FollowSpringArm->bUsePawnControlRotation = true;
	FollowSpringArm->bEnableCameraLag = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(FollowSpringArm);

	// 锁定组件
	FocusComp = CreateDefaultSubobject<UPlayerFocusComp>(TEXT("FocusComp"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::BeginPlay() called. %s"), *GetName());
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 更改ASC设置
	if (ASC)
	{
		ASC->SetOwnerActor(GetPlayerState());
	}

	// 设置阵营，用于AI感知
	SetGenericTeamId(1);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	static const FGameplayTag BackswingTag = SLAbilityTypes::GetAbilityStageTag(EAbilityStage::Backswing);

	const float MagnitudeSq = Value.GetMagnitudeSq();
	
	// 移动输入取消技能后摇
	if (MagnitudeSq > 0.0f && ASC && ASC->HasMatchingGameplayTag(BackswingTag))
	{
		UGameplayAbilityBase_ActiveAbility* ActiveAbility = Cast<UGameplayAbilityBase_ActiveAbility>(ASC->GetAnimatingAbility());
		ASC->CurrentMontageStop();
		ASC->RemoveLooseGameplayTag(BackswingTag);
		if (ActiveAbility && ActiveAbility->IsActive())
		{
			ActiveAbility->EndAbilityActively();
		}
	}

	// 更新移动输入
	MovementInput.X = Value[0];
	MovementInput.Y = Value[1];

	if (MagnitudeSq > 0.0f)
	{
		const FVector ControlForward = FRotator(0.0f, GetControlRotation().Yaw, 0.0f).Vector();
		AddMovementInput(ControlForward, Value[0]);

		static FRotator RightRot(0.0f, 90.0f, 0.0f);
		AddMovementInput(RightRot.RotateVector(ControlForward), Value[1]);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (Value.GetMagnitudeSq() > 0.0f)
	{
		AddControllerPitchInput(-Value[1]);
		AddControllerYawInput(Value[0]);
	}
}

void APlayerCharacter::Debug(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("[Debug] %f"), Value.GetMagnitude());
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::PossessedBy() called. Actor: %s, Controller: %s."), *GetNameSafe(this), *GetNameSafe(NewController));
}

void APlayerCharacter::UnPossessed()
{
	AController* OldController = Controller;

	// 移除默认IMC
	if (const APlayerController* PC = Cast<APlayerController>(OldController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(IMC_Default);
		}
	}

	Super::UnPossessed();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::SetupPlayerInputComponent() called."));

	// 添加操作-按键映射
	ApplyInputMappingContext(IMC_Default, 0);

	// 绑定操作方法
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		if (IA_Move)
		{
			EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
			EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &APlayerCharacter::Move);
		}
		if (IA_Look)
		{
			EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		}

		// 锁定功能
		if (FocusComp)
		{
			if (FocusComp->GetFocusAction())
			{
				EnhancedInputComponent->BindAction(FocusComp->GetFocusAction(), ETriggerEvent::Started, FocusComp, &UPlayerFocusComp::Focus);
			}
			if (FocusComp->GetSwitchTargetAction())
			{
				EnhancedInputComponent->BindAction(FocusComp->GetSwitchTargetAction(), ETriggerEvent::Started, FocusComp, &UPlayerFocusComp::SwitchTarget);
			}
		}
	}

	// 绑定技能和输入操作
	if (EnhancedInputComponent && ASC)
	{
		for (const auto AbilityClass : InitialActiveAbilities)
		{
			UGameplayAbilityBase_ActiveAbility::GiveAbilityTo(AbilityClass, ASC, EnhancedInputComponent);
		}
	}
}

FVector APlayerCharacter::GetWorldMovementIntent() const
{
	if (MovementInput.IsNearlyZero(0.0001f))
	{
		return FVector::ZeroVector;
	}
	FVector ControlDir(GetControlRotation().Vector());
	ControlDir.Z = 0;

	FVector MovementInputVec(MovementInput);
	MovementInputVec.Z = 0;
	return UKismetMathLibrary::MakeRotFromX(MovementInputVec).RotateVector(ControlDir).GetSafeNormal2D();
}

void APlayerCharacter::DebugAddGameplayTag(FGameplayTag DebugTag)
{
	if (ASC)
	{
		ASC->AddLooseGameplayTag(DebugTag);
	}
}

void APlayerCharacter::ApplyInputMappingContext(const UInputMappingContext* NewMappingContext, int32 Priority) const
{
	if (!IsValid(NewMappingContext))
	{
		return;
	}

	// 确保我们具有有效的玩家控制器。
	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// 从与我们的玩家控制器相关的本地玩家获取Enhanced Input本地玩家子系统。
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// 添加每个映射上下文及其优先级值。较高的值优先于较低的值。
			Subsystem->AddMappingContext(NewMappingContext, Priority);
		}
	}
}

void APlayerCharacter::RemapAbilityToInputAction()
{
	// TODO
}
