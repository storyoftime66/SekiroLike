// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MovementAgentInterface.h"

#include "Kismet/KismetSystemLibrary.h"


// Add default functionality here for any IMovementAgentInterface functions that are not pure virtual.
void IMovementAgentInterface::DrawDebugMovementIntent() const
{
	if (const AActor* Actor = Cast<AActor>(this))
	{
		const FVector Start = Actor->GetActorLocation();
		UKismetSystemLibrary::DrawDebugArrow(Actor, Start, Start + GetWorldMovementIntent() * 200.0f, 8.0f, FLinearColor::Red, 0.033f, 4.0f);
	}
}
