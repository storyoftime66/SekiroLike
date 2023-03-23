// Fill out your copyright notice in the Description page of Project Settings.


#include "DidItHitActorComponent.h"

#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UDidItHitActorComponent::UDidItHitActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
}


void UDidItHitActorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDidItHitActorComponent::LineTraceAndHandleHit(const FVector& Start, const FVector& End)
{
	TArray<FHitResult> HitResults;
	const EDrawDebugTrace::Type DrawDebug = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	UKismetSystemLibrary::LineTraceMulti(this, Start, End, TraceChannel, bTraceComplex, TempIgnoredActors, DrawDebug, HitResults, true);
	for (auto HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) and !HitActors.Contains(HitActor))
		{
			HitActors.Emplace(HitActor);
			TempIgnoredActors.Emplace(HitActor);
			OnHitActor.Broadcast(HitResult);
		}
	}
}

void UDidItHitActorComponent::UpdateLastKnownSocketLoc()
{
	check(StaticMeshComp);

	for (const auto& SocketName : SocketNames)
	{
		LastKnownSocketLocation.Add(SocketName, StaticMeshComp->GetSocketLocation(SocketName));
	}
}

void UDidItHitActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCanTrace)
	{
		check(IsValid(StaticMeshComp));

		if (bTraceSamePointAtDifferentTime)
		{
			if (LastKnownSocketLocation.Num() > 0)
			{
				for (const auto& SocketName1 : SocketNames)
				{
					FVector* StartPtr = LastKnownSocketLocation.Find(SocketName1);
					if (StartPtr != nullptr)
					{
						FVector End = StaticMeshComp->GetSocketLocation(SocketName1);
						LineTraceAndHandleHit(*StartPtr, End);
					}
				}
			}
		}

		if (bTraceDifferentPointAtSameTime)
		{
			for (int32 i = 0; i < SocketNames.Num() - 1; ++i)
			{
				FName SocketName1 = SocketNames[i];
				for (int32 j = i + 1; j < SocketNames.Num(); ++j)
				{
					FName SocketName2 = SocketNames[j];
					FVector Start = StaticMeshComp->GetSocketLocation(SocketName1);
					FVector End = StaticMeshComp->GetSocketLocation(SocketName2);
					LineTraceAndHandleHit(Start, End);
				}
			}
		}

		if (bTraceDifferentPointAtDifferentTime)
		{
			for (const auto& SocketName1 : SocketNames)
			{
				for (const auto& SocketName2 : SocketNames)
				{
					if (SocketName1 == SocketName2)
					{
						continue;
					}
					FVector* StartPtr = LastKnownSocketLocation.Find(SocketName2);
					if (StartPtr)
					{
						FVector End = StaticMeshComp->GetSocketLocation(SocketName1);
						LineTraceAndHandleHit(*StartPtr, End);
					}
				}
			}
		}

		// 更新插槽位置
		if (bTraceSamePointAtDifferentTime or bTraceDifferentPointAtDifferentTime)
		{
			UpdateLastKnownSocketLoc();
		}
	}
}

void UDidItHitActorComponent::SetupDitItHitComp(UStaticMeshComponent* NewStaticMeshComp)
{
	StaticMeshComp = NewStaticMeshComp;
	bCanTrace = false;
	SocketNames.Empty();
	LastKnownSocketLocation.Empty();
	if (IsValid(NewStaticMeshComp))
	{
		SocketNames = NewStaticMeshComp->GetAllSocketNames();
	}
}

void UDidItHitActorComponent::AddIgnoredActor(AActor* IgnoredActor)
{
	IgnoredActors.Add(IgnoredActor);
}

void UDidItHitActorComponent::AddIgnoredActors(TArray<AActor*> InIgnoredActor)
{
	IgnoredActors.Append(InIgnoredActor);
}

void UDidItHitActorComponent::ToggleTraceCheck(bool NewCanTrace)
{
	if (IsValid(StaticMeshComp) and SocketNames.Num() > 0)
	{
		// 在检测开始时清理上次的检测数据
		if (bCanTrace == false and NewCanTrace == true)
		{
			LastKnownSocketLocation.Empty();
			TempIgnoredActors.Empty();
			HitActors.Empty();
			TempIgnoredActors.Append(IgnoredActors);
		}
		SetComponentTickEnabled(NewCanTrace);
		bCanTrace = NewCanTrace;
	}
	else
	{
		bCanTrace = false;
		UE_LOG(LogTemp, Warning, TEXT("[Did It Hit] StaticMeshComp is invalid or there are not enough sockets."));
	}
}

TArray<AActor*> UDidItHitActorComponent::GetHitActors() const
{
	return HitActors;
}
