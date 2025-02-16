// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "CloneAssetManager.generated.h"


/*
	1. ��� ���� : AssetManager�� PrimaryAsset�� ���� Load, unload�� ���ش�.
	2. ���� : AssetManager�� ����ϴ� ������ static ConstructorHelper�� �̿��� objectfind�� ���� �ϵ��ڵ��� ���ϰ� ���ش�.
		- 100���� Asset�� ���� ��ο� ������ ������ ���� ���ΰ�?
		- ��� ����� ��� ���¿� ���� ��θ� ������ üũ�� ���ΰ�?
*/

/**
	SingleTon�� ���ϼ��� �����ؾ� �ϴ� ������ ����.

	AssetManager�� GEngine�� ĳ���� �Ǿ��־� �츮�� ���� CustomAssetManager�� Casting�� ���� ��ȯ�Ѵ�.

	return�� �ϴ� ������ Return ������ ���߱� ����.
**/

UCLASS()									//PrimaryAsset's load, unload
class CLONEPROJECT_API UCloneAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	UCloneAssetManager();

	//SingleTon�� ���� �⺻���� ���·�. �� �ϳ��� Instance�� �������� ���� Get�� �����
	static UCloneAssetManager& Get();

	virtual void StartInitialLoading() final;

	/*
		�ش� �Լ��� FScopelogTime�� ������� ������ ���� �Ǵ��� ���� ���.
	*/
	static bool ShouldLogAssetLoads();

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	//template�� �Լ��� ������ �ܰ迡�� �ڵ�� ��������� ������ �Լ��� ���Ǹ� .h���� �����Ѵ�.
	/*
		GetAsset�� ���� �ε����� BP Class�� Object�� �ε�
		- bKeepInMemory �ָ�
	*/
	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// [Thread-safe] �޸𸮿� �ε��� ���� ĳ��
	void AddLoadedAsset(const UObject* Asset);

	/*
		Asset���� Unreal ��ü�� Garbage Collection(.GC)���� �����ϴµ�, �ش� Set�� LoadedAsset�� �־�
		GC���� ����ó���� ���� �ʵ��� ��.
		��, ĳ�� �뵵�� ���.
	*/
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	//Object ������ Locking
	FCriticalSection SyncObject;
};

/*
	* Asset Type ����
	Asset : Blueprint, Sound	// �ϼ��� ��ü
	TSubclass : Ư�� Ŭ������ Subclass�� Ÿ������
	CDO : Ŭ������ �⺻ �ν��Ͻ�
	staticclass : MetaData (���÷���)				
	
	* ���÷����̶�? : ��Ÿ�� �ܰ迡 'Ŭ������ ���� ����'�� �� �� �ֵ��� �ϴ� ��.
					Ŭ���� ������ ����, �Լ� �̸� ��� �� �� �ְ�, ���÷����� ���� ���� ������ �Լ��� �ٲ� �� �ִ�.
	* Unreal������ UPROPERTY�� ���� Detail���� ������ ������ �� �ִ�.
		�� �������� �̸��̳� ������ �� �� �ֵ��� �ϴ� ���� ���÷����̴�.

	* Unreal Engine�� Build������ C#���� �� ������ C++���� ���÷��� ����� ���� �����̴�.
	* Unreal Engine�� UPROPERTY�� ���÷��� ����� �����ϱ� �����̰�, Generated_Body, ClassName.generated.h ������ �� �����̴�.
	* generated�� �ּ��� ���� �����ϸ� �ȵȴٴ� ������ �����Ѵ�.
	* 
	* MyClass::StaticClass()�� ���캸��, UClass�� ��ȯ�ϴ� ���� �� �� �ִ�. 
*/

template<typename AssetType>
AssetType* UCloneAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		//Loading�� �Ǿ��ִ�? -> �ٷ� �����´�.
		//Loading�� �� �Ǿ��ִ�? -> Null��ȯ
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load Asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			//LoadedAssets.Add(LoadedAsset); �� �ص� �Ǵµ� �� �ѹ� ������ �س��°�?
			//AddLoadedAsset�� ���� Lock�� �� �� �س���.
			//Lock�� ������ �� �� ��ϵǸ� ���� ������ �ʴ� �� UnLoad�� ���� ����, �� ĳ���� �ϴ� ��.
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}
/*
	TSubclass�� Staticclassó�� ��Ÿ ������(UClass)�� ������ �� �ִ�.
	TSubclass<T> �� T Type�� ��ӹ��� ��ü�鸸 ���� �� �ִ� ������ġ ���� ��.
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