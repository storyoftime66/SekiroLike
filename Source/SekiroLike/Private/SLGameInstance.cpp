// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameInstance.h"

void USLGameInstance::LoadSettings(bool LoadDefaultSettings/* = false */)
{
	FString JsonString;
	const FString LoadPath = LoadDefaultSettings ? DefaultSettingPath : CustomSettingPath;
	FFileHelper::LoadFileToString(JsonString, *(FPaths::ProjectContentDir() + LoadPath));
	auto JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	FJsonSerializer::Deserialize(JsonReader, JsonSettings);
}

void USLGameInstance::ApplySettings()
{
}

void USLGameInstance::SaveSettings()
{
	FString JsonString;
	auto JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonSettings.ToSharedRef(), JsonWriter);
	FFileHelper::SaveStringToFile(JsonString, *(FPaths::ProjectContentDir() + CustomSettingPath));
}
