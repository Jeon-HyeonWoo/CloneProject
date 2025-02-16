// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneExperienceManagerComponent.h"

#include "CloneProject/System/CloneAssetManager.h"
#include "CloneProject/GameModes/CloneExperienceDefinition.h"
#include "GameFeaturesSubsystemSettings.h"



void UCloneExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnCloneExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		//Load�� �Ǿ� �ִٸ�, Delegateȣ��.
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		//Load�� �ȵǾ� �ִٸ�, Delegate�� �߰�.
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
	/*
		MoveTemp ModernC++
		������ ������ �ް� �Ǹ�, ���縦 ���� �ʰ� �ٷ� �Ѱ��ش�. ������ ����.
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

	//GetDefault�� TSubclassOf�� �ƴ� ClassDefaultObject(CDO) �������� �����´�. 
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
		// OwnerNetMode�� NM_Standalone�̸� Client / Server �Ѵ� �ε��� �߰�
		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
		bool bLoadClinet = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
		bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
		if (bLoadClinet)
		{
			//LoadStateClient = TEXT("Client") �ܼ� TEXT�̴�.
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
		}
		if (bLoadServer)
		{
			//LoadStateServer = TEXT("LoadStateServer")
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
		}
	}

	//LoadComplete�� �Ǵ� Delegate����
	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);

	//ChangeBundleStateForPrimaryAssets : �񵿱�� �ε�, PrimaryAssetId�� Array�� �޾ƾ� ��.
	//SetCurrentExperience���� TryLoad�� �Ͽ�����, �� �ٽ� �񵿱� �ε��� �ϴ°�?
	//BundleAssetList���� BundleToLoad(FName)�� �´� Asset�� Loading�� �� �ֵ��� �ϴ� ���̴�.
	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority
	);

	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		//Handle�� ��ȿ�ϰ�, Handle�� Loading�� �Ϸᰡ �Ǿ��ٸ�, ��� Delegate�� Execute�ϴ� ���̴�.
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		//ASync�� Loading�ϴ� ���̱� ������, Loading�� �Ϸ���� �ʾҴٸ�, Complete������ Delegate�� ȣ�������.
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
		//loading�� �������� ��, ȣ�������. 
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
		{
			//Delegate�� �����ִٸ� ȣ�������.
			OnAssetsLoadedDelegate.ExecuteIfBound();
		}
		));

	}
	
	static int32 StartExperienceLoad_FrameNumber = GFrameNumber;

}

void UCloneExperienceManagerComponent::OnExperienceLoadComplete()
{
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	//���� Loading�� ���̵��� ��������, 
	OnExperienceFullLoadCompleted();
}

void UCloneExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != ECloneExperienceLoadState::Loaded);

	LoadState = ECloneExperienceLoadState::Loaded;

	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}
