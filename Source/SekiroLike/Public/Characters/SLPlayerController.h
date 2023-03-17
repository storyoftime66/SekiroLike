// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "SLPlayerController.generated.h"

UCLASS()
class SEKIROLIKE_API ASLPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
protected:
	FGenericTeamId TeamID = 1;

public:
	// Sets default values for this actor's properties
	ASLPlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//~ IGenericTeamAgentInterface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
};
