// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneGamePlayTags.h"
#include "CloneLogChannels.h"
#include "GameplayTagsManager.h"

FCloneGameplayTags FCloneGameplayTags::GameplayTags;

void FCloneGameplayTags::InitializeNatvieTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(Manager);
}

void FCloneGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}

void FCloneGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	/* 
		GameFrameworkComponentManager Init State Tags
	*/
	AddTag(InitState_Spawned, "InitState.Spawned", "1: Actor/Component has initially spawned and can be extended");
	AddTag(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for Initialized");
	AddTag(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component");
	AddTag(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");

	/*
		Enhanced Input Tags
	*/
	AddTag(InputTag_Move, "InputTag.Move", "");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "");
}
