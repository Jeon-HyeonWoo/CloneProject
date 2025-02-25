// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonSessionSubSystem.h"
#include "Engine/AssetManager.h"
#include "Engine/LocalPlayer.h"

FString UCommonSession_HostSessionRequest::GetMapName() const
{
	FAssetData MapAssetData;
	if (UAssetManager::Get().GetPrimaryAssetData(MapID, MapAssetData))
	{
		return MapAssetData.PackageName.ToString();
	}
	return FString();
}

FString UCommonSession_HostSessionRequest::ConstructTravelURL() const
{
	/*
		if) 
		MapID = ABC
		Experience = FPS Mode.
		-> ABC ? Experience= FPS Mode
		������ �ϴ� ����? : Map��� �տ� �߰��Ͽ� ���� TravelURL������ ����
		? : ?�տ��� ?�ڿ� ���� �����´�.
		= : 

		Key�� Experience�� �ְ�, Value�� FPSMode�� �ִ´�. �̷��� �ϳ��� ������ �����.

		���� : Map�̵��� ���� URL�� Unreal ���Ŀ� �°� ����� ����.
	*/
	FString CombinedExtraArgs;

	//Loop Arguement
	for (const auto& ExtraArg : ExtraArgs)
	{
		if (ExtraArg.Key.IsEmpty())
		{
			continue;
		}

		if (ExtraArg.Value.IsEmpty())
		{
			CombinedExtraArgs += FString::Printf(TEXT("?%s"), *ExtraArg.Key);
		}
		else
		{
			CombinedExtraArgs += FString::Printf(TEXT("?%s=%s"), *ExtraArg.Key, *ExtraArg.Value);
		}
	}

	return FString::Printf(TEXT("%s%s"), *GetMapName(), *CombinedExtraArgs);
}

void UCommonSessionSubSystem::HostSession(APlayerController* HostingPlayer, UCommonSession_HostSessionRequest* Request)
{
	/*
	* Request�� ��û��, HostSession�� ������ ����!
	*/
	ULocalPlayer* LocalPlayer = (HostingPlayer != nullptr) ? HostingPlayer->GetLocalPlayer() : nullptr;
	if (!LocalPlayer)
	{
		return;
	}

	/*
	* ClinetTravel -> �ٸ� Dedicated ������ �̵�
	* ServerTravel -> �ش缭�� Client��ü �ε�
	*/
	GetWorld()->ServerTravel(Request->ConstructTravelURL());


}
