// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "CloneExperienceManagerComponent.generated.h"

class UCloneExperienceDefinition;

/**
*  상속 단계
 * ActorComponent : GameFrameWorkCompoent : GameStateComponent (Modular GamePlay)
 * Component는 어디에 부착 시킬 수 있음.
 * ActorComponent를 상속 받아서 Actor에 부착시킬 수 있지만, Modular Gameplay에서는 한 번더 랩핑하여 GameState에도 부착이 가능하게 함.
 * 
 * 이름 그래도 GameState에 부착시킬 수 있는 Component이다.
 * 
 * GameState에 여러 Component를 탈, 부착시켜 확장성에 용이하다. Unity에서는 일반적인 방법. 
 * 단점으로는 Debug가 힘들고, Filing이 힘들다.
 */

/*
	ExperienceManager에는 CurrentExperience를 들고 있지만 Load가 된 것은 아니다.
	GameMode에서 ExperienceManager에게 Load해달라는 요청이 들어오면 그 때 시작한다.
	CurrentExperience의 Load상태를 확인하기 위해 enumclass를 사용한다.
	완료되는 시점을 체크하기위해 Delegate를 사용한다.
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
