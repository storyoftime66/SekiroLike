// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SLAIController.h"


// Sets default values
ASLAIController::ASLAIController()
{
	PrimaryActorTick.bCanEverTick = false;
	SetGenericTeamId(10);
}

// Called when the game starts or when spawned
void ASLAIController::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASLAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

