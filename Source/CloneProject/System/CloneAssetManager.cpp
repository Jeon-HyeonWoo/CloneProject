// Fill out your copyright notice in the Description page of Project Settings.

#include "CloneAssetManager.h"
#include "CloneProject/CloneLogChannels.h"
#include "CloneProject/CloneGamePlayTags.h"



UCloneAssetManager::UCloneAssetManager()
{
}

UCloneAssetManager& UCloneAssetManager::Get()
{
	check(GEngine);

	if (UCloneAssetManager* Singleton = Cast<UCloneAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogClone, Fatal, TEXT("invalid AssetManagerClassName in DefaultEngine.ini(project settings); it must be CloneAssetManager"));

	return *NewObject<UCloneAssetManager>();
}

/*
	���� ���� ����� Development Editor�ε�,
	����뿡 ���� �͵��� �ּ�ȭ �ϴ� ���, �ӵ��� ��������.
	�ش� ��ũ�� ���� �� �κ��� �����ϰ� Development�� �����Ѵ�.
	���� �ʿ信 ���� �� �κ��� ���带 �� �ϰ� �����Ѵٴ� ��.
*/
// this method is Called, Before open Editor
PRAGMA_DISABLE_OPTIMIZATION
void UCloneAssetManager::StartInitialLoading()
{
	//�⺻������ Unreal�� Multi Thead �ڵ��� �����Ѵ�.
	//�� Editor�� ������ �� ������ Multi Thread��Ȳ�� safe ���� �ʴ´�.
	Super::StartInitialLoading();

	//Add All Native Tags
	FCloneGameplayTags::InitializeNatvieTags();
}
PRAGMA_ENABLE_OPTIMIZATION


bool UCloneAssetManager::ShouldLogAssetLoads()
{
	/*
		FCommandLine : Cmd�� ������ ����ٿ� ���� char* ���� Get.
		Param�� CommandLine�� TEXT���� �� 
	*/
	/*
		Debug Arguments�� -LoadAssetLoads�� �߰��Ͽ� �ش� Args�� ���� ����, �񵿱� ������ �ε� ����� �����Ѵ�.
	*/
	const TCHAR* CommandLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(CommandLineContent, TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

/*
	1. ����� Load
	Task_A �� Task_B�� �ִٰ� ����.
	Task_A�� Load�� �Ϸ�Ǳ� ������, Task_B�� �Ѿ �� ����. 
	- ���� : �ӵ��� ������. ���� ���� ĳġ�� ����.
	- ���� : ���Ӱ��� �� Ŭ���̾�Ʈ�� �����ؼ� ����� ��� �������� ���� �� �ִ�.

	2. �񵿱�� Load
	Task_A�� Load�� �����ɷ� Task_B�� ����� ���� ���� ���� ���.
	Thread�� ���� Task_A�� Load�� ��Ƶΰ�, Task_B�� �Ѿ�� �Ѵ�.
	- ������ ���� �񵿱�� Loading�� ���� ��찡 ����.

	�׷��ٸ� �� �Լ��� ���� ���Ǵ� ������� ����ϴ� client Load�� ��������� ������ �ʿ��� ��츦 ���ؼ��̴�.
	* AssetManager�� �̹� �����ϴ� �Լ�����, �� ���� �����Ͽ��°�? -> �ð��ʸ� ��� �����̴�.
	* ���� �ε尡� ���� �ɸ��� �ε��� �ִٸ�, ���� �ε��� ����� �ٲ� �ʿ䰡 �ֱ� ������ �ð��ʸ� �����Ѵ�.
*/
/*
	Asset_A, Asset_B, Asset_C�� �ִٰ� ����. �ش� Asset�� Load�ϴ� ����.
	������ Asset�� ������ ��ΰ� �����ϰ�. 
	FSoftObjectPath�� �� ����� ���̴�.
*/
UObject* UCloneAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogtimePtr;

		if (ShouldLogAssetLoads())
		{
			//����ε尡 ����Ǹ� �� �ʰ� �ɸ������� ���� Check.
			LogtimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsValid())
		{
			//GetStreamableManager : ������ ���� �񵿱�ε带 ���� Manager.
			//�Ʒ��� ���� Synchrounus�� ����ϴ� AssetPath�� ������ Synchronous�� �����Ѵ�.
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}
		//AssetManager�� Engine���� ����� ���� ���� ��츦 ���.
		//TryLoad�� �ּ��� �ſ� �����ٰ� �Ǿ��ִ�. �׷��� ���� ������� �ʴ� ����� ��õ.
		//TryLoad�ڵ忡 �׻� StaticLoadObject�� �ϱ����� StaticFindObject�� �ϴµ�, �̴� �޸𸮿� ĳ���� �Ǿ��ִ��� üũ�� ���ش�.
		return AssetPath.TryLoad();
	}

	return nullptr;
	
}

void UCloneAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		//���� FCriticalSection�� ������ Safe��, LoadedAssets�� �� ���� ������ �ǵ��� Lock�� �ɾ��ִ� ���̴�.
		//AssetManager�� Multi Thread ȯ�濡 safe���� ��������, Editor�� ������ ���� �����Ǵ� AssetManager�� ���� ������ġ�� �ɾ�� ��.
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}
