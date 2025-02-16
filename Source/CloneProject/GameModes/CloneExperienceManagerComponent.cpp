// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneExperienceManagerComponent.h"

#include "CloneProject/System/CloneAssetManager.h"
#include "CloneProject/GameModes/CloneExperienceDefinition.h"
#include "GameFeaturesSubsystemSettings.h"



void UCloneExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnCloneExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		//Load가 되어 있다면, Delegate호출.
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		//Load가 안되어 있다면, Delegate에 추가.
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
	/*
		MoveTemp ModernC++
		우측값 참조로 받게 되면, 복사를 하지 않고 바로 넘겨준다. 복사비용 절감.
	*/
}

void UCloneExperienceManagerComponent::ServerSetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UCloneAssetManager& AssetManager = UCloneAssetManager::Get();

	TSubclassOf<UCloneExperienceDefinition> AssetClass;
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
		AssetClass = Cast<UClass>(AssetPath.TryLoad()); //TryLoad() = Syncronus Loading
	}

	//GetDefault는 TSubclassOf가 아닌 ClassDefaultObject(CDO) 형식으로 가져온다. 
	const UCloneExperienceDefinition* Experience = GetDefault<UCloneExperienceDefinition>(AssetClass);
	check(Experience != nullptr);
	check(CurrentExperience == nullptr)
	{
		CurrentExperience = Experience;
	}

	
	StartExperienceLoad();
}

void UCloneExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	//UnLoad Check
	check(LoadState == ECloneExperienceLoadState::Unload);

	//Set LoadState Loading
	LoadState = ECloneExperienceLoadState::Loading;

	UCloneAssetManager& AssetManager = UCloneAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	//CureentExperience = FPrimaryAssetType(CloneExperienceDefinition), FName(BP_CloneDefualtExperience)
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());
	
	// Load Assets Associated with the experience
	/* 
		
	*/
	TArray<FName> BundlesToLoad;
	{
		// OwnerNetMode가 NM_Standalone이면 Client / Server 둘다 로딩에 추가
		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
		bool bLoadClinet = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
		bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
		if (bLoadClinet)
		{
			//LoadStateClient = TEXT("Client") 단순 TEXT이다.
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
		}
		if (bLoadServer)
		{
			//LoadStateServer = TEXT("LoadStateServer")
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
		}
	}

	//LoadComplete가 되는 Delegate생성
	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);

	//ChangeBundleStateForPrimaryAssets : 비동기로 로딩, PrimaryAssetId의 Array를 받아야 함.
	//SetCurrentExperience에서 TryLoad를 하였으나, 왜 다시 비동기 로딩을 하는가?
	//BundleAssetList에서 BundleToLoad(FName)과 맞는 Asset만 Loading할 수 있도록 하는 것이다.
	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority
	);

	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		//Handle이 유효하고, Handle의 Loading이 완료가 되었다면, 즉시 Delegate를 Execute하는 것이다.
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		//ASync로 Loading하는 것이기 때문에, Loading이 완료되지 않았다면, Complete했을때 Delegate를 호출해줘라.
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
		//loading에 실패했을 때, 호출해줘라. 
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
		{
			//Delegate가 남아있다면 호출해줘라.
			OnAssetsLoadedDelegate.ExecuteIfBound();
		}
		));

	}
	
	static int32 StartExperienceLoad_FrameNumber = GFrameNumber;

}

void UCloneExperienceManagerComponent::OnExperienceLoadComplete()
{
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	//향후 Loading할 것이들이 많아지면, 
	OnExperienceFullLoadCompleted();
}

void UCloneExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != ECloneExperienceLoadState::Loaded);

	LoadState = ECloneExperienceLoadState::Loaded;

	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}
