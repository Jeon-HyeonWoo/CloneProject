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
	현재 빌드 방법이 Development Editor인데,
	디버깅에 관한 것들을 최소화 하는 대신, 속도가 빨라진다.
	해당 매크로 사용시 이 부분을 제외하고 Development로 빌드한다.
	따라서 필요에 따라 이 부분은 빌드를 꼭 하고 가야한다는 뜻.
*/
// this method is Called, Before open Editor
PRAGMA_DISABLE_OPTIMIZATION
void UCloneAssetManager::StartInitialLoading()
{
	//기본적으로 Unreal은 Multi Thead 코딩을 지원한다.
	//단 Editor가 켜지기 전 까지는 Multi Thread상황에 safe 하지 않는다.
	Super::StartInitialLoading();

	//Add All Native Tags
	FCloneGameplayTags::InitializeNatvieTags();
}
PRAGMA_ENABLE_OPTIMIZATION


bool UCloneAssetManager::ShouldLogAssetLoads()
{
	/*
		FCommandLine : Cmd에 실행한 명령줄에 대한 char* 값을 Get.
		Param에 CommandLine에 TEXT값을 비교 
	*/
	/*
		Debug Arguments에 -LoadAssetLoads를 추가하여 해당 Args를 사용시 동기, 비동기 식으로 로드 방법을 구분한다.
	*/
	const TCHAR* CommandLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(CommandLineContent, TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

/*
	1. 동기식 Load
	Task_A 와 Task_B가 있다고 가정.
	Task_A의 Load가 완료되기 전까진, Task_B로 넘어갈 수 없다. 
	- 장점 : 속도가 빠르다. 로직 순서 캐치가 쉽다.
	- 단점 : 게임같이 다 클라이언트가 공용해서 사용할 경우 불편함을 겪을 수 있다.

	2. 비동기식 Load
	Task_A의 Load가 오래걸려 Task_B의 기능을 당장 쓰지 못할 경우.
	Thread를 통해 Task_A의 Load를 잡아두고, Task_B로 넘어가게 한다.
	- 게임의 경우는 비동기식 Loading을 쓰는 경우가 많다.

	그렇다면 이 함수의 존재 의의는 동기식을 사용하는 client Load등 동기식으로 진행이 필요할 경우를 위해서이다.
	* AssetManager에 이미 존재하는 함수지만, 왜 따로 구현하였는가? -> 시간초를 재기 위함이다.
	* 동기 로드가운데 오래 걸리는 로딩이 있다면, 동기 로드의 방식을 바꿀 필요가 있기 때문에 시간초를 설정한다.
*/
/*
	Asset_A, Asset_B, Asset_C가 있다고 가정. 해당 Asset을 Load하는 과정.
	각각의 Asset은 각각의 경로가 존재하고. 
	FSoftObjectPath는 그 경로의 값이다.
*/
UObject* UCloneAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogtimePtr;

		if (ShouldLogAssetLoads())
		{
			//동기로드가 실행되면 몇 초가 걸리는지에 대한 Check.
			LogtimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsValid())
		{
			//GetStreamableManager : 실제로 동기 비동기로드를 돕는 Manager.
			//아래의 경우는 Synchrounus를 사용하니 AssetPath를 실제로 Synchronous로 실행한다.
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}
		//AssetManager가 Engine에서 만들어 지지 않을 경우를 대비.
		//TryLoad는 주석에 매우 느리다고 되어있다. 그래서 비교적 사용하지 않는 방법을 추천.
		//TryLoad코드에 항상 StaticLoadObject를 하기전에 StaticFindObject를 하는데, 이는 메모리에 캐싱이 되어있는지 체크를 해준다.
		return AssetPath.TryLoad();
	}

	return nullptr;
	
}

void UCloneAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		//변수 FCriticalSection은 일종의 Safe로, LoadedAssets에 한 개씩 저장이 되도록 Lock을 걸어주는 것이다.
		//AssetManager는 Multi Thread 환경에 safe하지 않음으로, Editor가 열리기 전에 생성되는 AssetManager에 대해 안전장치를 걸어둔 것.
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}
