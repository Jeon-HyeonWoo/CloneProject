// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "CloneAssetManager.generated.h"


/*
	1. 사용 이유 : AssetManager는 PrimaryAsset에 대한 Load, unload를 해준다.
	2. 목적 : AssetManager를 사용하는 이유는 static ConstructorHelper를 이용한 objectfind를 사용시 하드코딩을 피하게 해준다.
		- 100개의 Asset에 대한 경로와 설정을 일일이 해줄 것인가?
		- 경로 변경시 모든 에셋에 대한 경로를 일일이 체크할 것인가?
*/

/**
	SingleTon은 유일성을 보장해야 하는 디자인 패턴.

	AssetManager는 GEngine에 캐싱이 되어있어 우리가 만든 CustomAssetManager에 Casting한 이후 반환한다.

	return을 하는 이유는 Return 형식을 맞추기 위함.
**/

UCLASS()									//PrimaryAsset's load, unload
class CLONEPROJECT_API UCloneAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	UCloneAssetManager();

	//SingleTon에 가장 기본적인 형태로. 단 하나의 Instance를 가져오기 위해 Get을 만든다
	static UCloneAssetManager& Get();

	virtual void StartInitialLoading() final;

	/*
		해당 함수는 FScopelogTime을 사용할지 않을지 유무 판단을 위해 사용.
	*/
	static bool ShouldLogAssetLoads();

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	//template의 함수는 컴파일 단계에서 코드로 만들어지기 때문에 함수의 정의를 .h에서 정의한다.
	/*
		GetAsset은 정적 로딩으로 BP Class와 Object를 로딩
		- bKeepInMemory 주목
	*/
	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// [Thread-safe] 메모리에 로딩된 에셋 캐싱
	void AddLoadedAsset(const UObject* Asset);

	/*
		Asset들은 Unreal 자체의 Garbage Collection(.GC)에서 관리하는데, 해당 Set에 LoadedAsset을 넣어
		GC에서 삭제처리를 하지 않도록 함.
		즉, 캐싱 용도로 사용.
	*/
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	//Object 단위의 Locking
	FCriticalSection SyncObject;
};

/*
	* Asset Type 차이
	Asset : Blueprint, Sound	// 완성된 객체
	TSubclass : 특정 클래스의 Subclass의 타입정보
	CDO : 클래스의 기본 인스턴스
	staticclass : MetaData (리플렉션)				
	
	* 리플렉션이란? : 런타임 단계에 '클래스에 대한 정보'를 알 수 있도록 하는 것.
					클래스 내부의 변수, 함수 이름 등등 알 수 있고, 리플렉션을 통해 내부 변수나 함수를 바꿀 수 있다.
	* Unreal에서는 UPROPERTY를 통해 Detail에서 변수를 수정할 수 있다.
		이 변수들의 이름이나 변경을 할 수 있도록 하는 것이 리플렉션이다.

	* Unreal Engine의 Build파일이 C#으로 된 이유는 C++에는 리플렉션 기능이 없기 때문이다.
	* Unreal Engine의 UPROPERTY는 리플렉션 기능을 제공하기 위함이고, Generated_Body, ClassName.generated.h 파일이 이 때문이다.
	* generated는 주석에 절대 수정하면 안된다는 문구가 존재한다.
	* 
	* MyClass::StaticClass()를 살펴보면, UClass를 반환하는 것을 알 수 있다. 
*/

template<typename AssetType>
AssetType* UCloneAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		//Loading이 되어있다? -> 바로 가져온다.
		//Loading이 안 되어있다? -> Null반환
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load Asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			//LoadedAssets.Add(LoadedAsset); 을 해도 되는데 왜 한번 랩핑을 해놨는가?
			//AddLoadedAsset을 보면 Lock을 한 번 해놨다.
			//Lock의 이유는 한 번 등록되면 직접 내리지 않는 한 UnLoad가 되지 않음, 즉 캐싱을 하는 것.
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}
/*
	TSubclass도 Staticclass처럼 메타 데이터(UClass)를 저장할 수 있다.
	TSubclass<T> 는 T Type을 상속받은 개체들만 받을 수 있는 안전장치 같은 것.
*/
template<typename AssetType>
TSubclassOf<AssetType> UCloneAssetManager::GetSubclass(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load Asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}