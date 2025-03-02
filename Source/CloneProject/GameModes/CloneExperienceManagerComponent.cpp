// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneExperienceManagerComponent.h"

#include "CloneProject/System/CloneAssetManager.h"
#include "CloneProject/GameModes/CloneExperienceDefinition.h"
#include "GameFeaturesSubsystemSettings.h"
#include "GameFeaturesSubsystem.h"
#include "CloneExperienceActionSet.h"
#include "GameFeatureAction.h"




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

	check(LoadState == ECloneExperienceLoadState::Loading);
	check(CurrentExperience);

	//���� Ȱ��ȭ �� GameFeature PlugIn�� URL�� Clear
	GameFeaturePlugInURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
		{
			for (const FString& PluginName : FeaturePluginList)
			{
				FString PluginURL;
				//GameFeatureSubsystem : public EngineSubsystem
				if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
				{
					This->GameFeaturePlugInURLs.AddUnique(PluginURL);
				}
			}
		};

	//GameFeatureToEnable�� �ִ� PlugIn�� �ϴ� Ȱ��ȭ�� GameFeature Plugin�ĺ������� ���
	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);

	//GameFeaturePluginURLs�� ��ϵ� Plugin�� �ε� �� Ȱ��ȭ
	NumGameFeaturePluginsLoading = GameFeaturePlugInURLs.Num();

	//Num > 0
	if (NumGameFeaturePluginsLoading)
	{
		LoadState = ECloneExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePlugInURLs)
		{
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		//�ش� �Լ��� �Ҹ��� ���� �ռ� ���Ҵ� StreamableDelegateDelayHelper�� ���� �Ҹ�
		OnExperienceFullLoadCompleted();
	}

	//���� Loading�� ���̵��� ��������, 
	
}

void UCloneExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// �� GameFeature Plugin�� �ε��� �� ��, �ش� �Լ��� �ݹ����� �Ҹ���.
	NumGameFeaturePluginsLoading--;
	if (NumGameFeaturePluginsLoading == 0)
	{
		//GmaeFeaturePlugin �ε��� �� ������ ���, ������� Loaded�μ�, OnExperienceFullLoadCompleted�� ȣ��
		OnExperienceFullLoadCompleted();
	}
}

/*
* GameFeature�� ������ Load�ܰ��, Loaded�� �Ϸ�ƴٴ� ���� ������� �� �ִ�. ���� ���⼭ ExecutingActions�ܰ� Ȱ��
*/
void UCloneExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != ECloneExperienceLoadState::Loaded);

	{
		LoadState = ECloneExperienceLoadState::ExecutingActions;

		FGameFeatureActivatingContext Context;
		{
			const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
			if (ExistingWorldContext)
			{
				//Context�� WorldContext�� �ִ´�. � �������� ������ ���ֱ� ����
				Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
			}
		}
		
		//Context�� ĸ���ϰ�, Features�� Actions���� ������ ������.
		auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				//�ش� Actions�� ��ȸ�ϸ鼭
				for (UGameFeatureAction* Action : ActionList)
				{
					if (Action)
					{
						//���, �ε�, Ȱ��ȭ(Context�� �ʿ�) -> ActionȰ��ȭ!
						Action->OnGameFeatureRegistering();
						Action->OnGameFeatureLoading();
						Action->OnGameFeatureActivating(Context);
					}
				}
			};

		//1. Experience�� �Ϲ� Actions
		ActivateListOfActions(CurrentExperience->Actions);

		//2. Experience�� ActionSets
		for (const TObjectPtr<UCloneExperienceActionSet>& ActionSet : CurrentExperience->ActionSet)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	

	LoadState = ECloneExperienceLoadState::Loaded;

	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

const UCloneExperienceDefinition* UCloneExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	//Loading is finished?
	check(LoadState == ECloneExperienceLoadState::Loaded);
	//CurrentExperience isn't nullptr?
	check(CurrentExperience != nullptr);

	return CurrentExperience;
}
