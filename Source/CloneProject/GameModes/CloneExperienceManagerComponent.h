// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "CloneExperienceManagerComponent.generated.h"

class UCloneExperienceDefinition;

/**
*  ��� �ܰ�
 * ActorComponent : GameFrameWorkCompoent : GameStateComponent (Modular GamePlay)
 * Component�� ��� ���� ��ų �� ����.
 * ActorComponent�� ��� �޾Ƽ� Actor�� ������ų �� ������, Modular Gameplay������ �� ���� �����Ͽ� GameState���� ������ �����ϰ� ��.
 * 
 * �̸� �׷��� GameState�� ������ų �� �ִ� Component�̴�.
 * 
 * GameState�� ���� Component�� Ż, �������� Ȯ�强�� �����ϴ�. Unity������ �Ϲ����� ���. 
 * �������δ� Debug�� �����, Filing�� �����.
 */

/*
	ExperienceManager���� CurrentExperience�� ��� ������ Load�� �� ���� �ƴϴ�.
	GameMode���� ExperienceManager���� Load�ش޶�� ��û�� ������ �� �� �����Ѵ�.
	CurrentExperience�� Load���¸� Ȯ���ϱ� ���� enumclass�� ����Ѵ�.
	�Ϸ�Ǵ� ������ üũ�ϱ����� Delegate�� ����Ѵ�.
*/

enum class ECloneExperienceLoadState
{
	Unload,
	Loading,
	LoadingGameFeatures,
	ExecutingActions,
	Loaded,
	Deactivating,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCloneExperienceLoaded, const UCloneExperienceDefinition*);

UCLASS()
class CLONEPROJECT_API UCloneExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
	
public:

	bool IsExperienceLoaded() { return (LoadState == ECloneExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

	void CallOrRegister_OnExperienceLoaded(FOnCloneExperienceLoaded::FDelegate&& Delegate);

	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();
	const UCloneExperienceDefinition* GetCurrentExperienceChecked() const;

public: 
	
	UPROPERTY()
	TObjectPtr<const UCloneExperienceDefinition> CurrentExperience;

	/* Observe Experience LoadState, Initial Value = Unload */
	ECloneExperienceLoadState LoadState = ECloneExperienceLoadState::Unload;

	/*if Loading is finished, Broadcasting delegate*/
	FOnCloneExperienceLoaded OnExperienceLoaded;

	/* Activated GameFeature Plugins */
	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePlugInURLs;
};
