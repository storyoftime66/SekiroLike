// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
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

	// 组件创建
	FollowSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FollowSpringArm"));
	FollowSpringArm->SetupAttachment(RootComponent);
	FollowSpringArm->TargetArmLength = 400.0f;
	FollowSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	FollowSpringArm->bUsePawnControlRotation = true;
	FollowSpringArm->bEnableCameraLag = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(FollowSpringArm);

	// 输入操作，用于绑定
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveObj(TEXT("/Game/SekiroLike/Blueprints/Input/IA_Move"));
	IA_Move = IA_MoveObj.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LookObj(TEXT("/Game/SekiroLike/Blueprints/Input/IA_Look"));
	IA_Look = IA_LookObj.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_AttackObj(TEXT("/Game/SekiroLike/Blueprints/Input/IA_Attack"));
	IA_Attack = IA_AttackObj.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_BlockObj(TEXT("/Game/SekiroLike/Blueprints/Input/IA_Block"));
	IA_Block = IA_BlockObj.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_FocusObj(TEXT("/Game/SekiroLike/Blueprints/Input/IA_Focus"));
	IA_Focus = IA_FocusObj.Object;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMCObj(TEXT("/Game/SekiroLike/Blueprints/Input/MC_PlayerDefault"));
	DefaultMC = DefaultMCObj.Object;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ASC)
	{
		ASC->SetOwnerActor(GetPlayerState());
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	static FRotator RightRot(0.0f, 90.0f, 0.0f);

	MovementInput.X = Value[0];
	MovementInput.Y = Value[1];
	
	if (Value.GetMagnitudeSq() > 0.0f)
	{
		const FVector ControlForward = FRotator(0.0f, GetControlRotation().Yaw, 0.0f).Vector();
		AddMovementInput(ControlForward, Value[0]);
		AddMovementInput(RightRot.RotateVector(ControlForward), Value[1]);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (Value.GetMagnitudeSq() > 0.0f and !IsValid(FocusedEnemy))
	{
		AddControllerPitchInput(-Value[1]);
		AddControllerYawInput(Value[0]);
	}
}

void APlayerCharacter::Attack()
{
}

void APlayerCharacter::Block()
{
}

void APlayerCharacter::Focus()
{
	check(GetController());

	if (IsValid(FocusedEnemy))
	{
		FocusedEnemy = nullptr;
	}
	else
	{
		// 检测并获取可以锁定的敌人
		check(GetOwner());
		check(GetWorld());

		FVector CamLoc;
		FRotator CamRot;
		GetController()->GetPlayerViewPoint(CamLoc, CamRot);
		// TODO: 锁定范围是否需要写死
		const FVector Start = CamLoc + CamRot.Vector() * 400.0f;
		const FVector End = Start + CamRot.Vector() * 600.0f;

		// 下面代码修改自 UKismetSystemLibrary::SphereTraceSingle
		FHitResult OutHit;
		FCollisionQueryParams Params(FName("PlayerFocusAction"), SCENE_QUERY_STAT_ONLY(Focus), true);
		Params.bReturnPhysicalMaterial = false;
		Params.bReturnFaceIndex = false;
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->SweepSingleByObjectType(
			OutHit, Start, End, FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn),
			FCollisionShape::MakeSphere(300.0f), Params);
		if (bHit)
		{
			FocusedEnemy = Cast<ACharacter>(OutHit.GetActor());
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	APlayerController* PC = GetController<APlayerController>();

	if (IsValid(FocusedEnemy))
	{
		// 旋转角色
		GetCharacterMovement()->bOrientRotationToMovement = false;
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FocusedEnemy->GetActorLocation());
		const FRotator ActorTargetRot(0.0f, LookAtRot.Yaw, 0.0f);
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), ActorTargetRot, DeltaTime, 10.0f));
		// 旋转视角
		if (PC)
		{
			const FRotator ControlTargetRot(-15.0f, LookAtRot.Yaw, 0.0f);
			PC->SetControlRotation(FMath::RInterpTo(PC->GetControlRotation(), ControlTargetRot, DeltaTime, 4.0f));
		}
	}
	else
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void APlayerCharacter::UnPossessed()
{
	AController* OldController = Controller;
	if (const APlayerController* PC = Cast<APlayerController>(OldController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(DefaultMC);
		}
	}
	Super::UnPossessed();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	ApplyInputMappingContext(DefaultMC, 0);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
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
		if (IA_Attack)
		{
			EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &APlayerCharacter::Attack);
		}
		if (IA_Block)
		{
			EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Started, this, &APlayerCharacter::Block);
		}
		if (IA_Focus)
		{
			EnhancedInputComponent->BindAction(IA_Focus, ETriggerEvent::Started, this, &APlayerCharacter::Focus);
		}
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
