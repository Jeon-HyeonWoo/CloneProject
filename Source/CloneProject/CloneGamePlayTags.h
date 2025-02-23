// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "GamePlayTagContainer.h"

/* Forward Declaration*/
class UGameplayTagsManager;

/*
	CloneGameplayTags 
	- 'Singleton Containing' Native Gameplay Tags
*/
struct FCloneGameplayTags
{
	/*
		Static methods
	*/
	static const FCloneGameplayTags& Get() { return GameplayTags; }
	/* this methods called AssetManager_StartInitialLoading */
	static void InitializeNatvieTags();

	/*
		member methods
	*/

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddAllTags(UGameplayTagsManager& Manager);

	/*
		Initialize Steps 
	*/
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;

	/*
		Enhanced Input Tags
	*/
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;

private:

	static FCloneGameplayTags GameplayTags;
};