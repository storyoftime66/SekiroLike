// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLGameInstance.generated.h"

class UInputMappingContext;
/**
 * 
 */
UCLASS()
class SEKIROLIKE_API USLGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
#pragma region GAME_SETTINGS

	UPROPERTY()
	UGameUserSettings* GameSettings;
	FString CustomSettingPath;
	FString DefaultSettingPath;
	/** 游戏设置，以Json形式保存，不包括键位设置 */
	TSharedPtr<FJsonObject> JsonSettings;
	/** 默认的键位设置 */
	UPROPERTY()
	UInputMappingContext* DefaultIMC;
	/** 当前键位设置 */
	UPROPERTY()
	UInputMappingContext* CurrentIMC;
	/** 修改了，但未应用的键位设置 */
	UPROPERTY()
	UInputMappingContext* NotAppliedIMC;

	/** 从Json文件中加载设置 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|Settings")
	void LoadSettings(bool LoadDefaultSettings = false);

	/** 应用游戏设置 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|Settings")
	void ApplySettings();

	/** 保存设置到Json文件 */
	UFUNCTION(BlueprintCallable, Category="SekiroLike|Settings")
	void SaveSettings();


#pragma endregion
};
