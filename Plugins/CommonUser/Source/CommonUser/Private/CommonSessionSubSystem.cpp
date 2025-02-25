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
		이짓을 하는 이유? : Map경로 앞에 추가하여 최종 TravelURL생성을 위해
		? : ?앞에서 ?뒤에 것을 가져온다.
		= : 

		Key로 Experience를 넣고, Value를 FPSMode를 넣는다. 이렇게 하나의 조합을 만든다.

		정리 : Map이동을 위한 URL을 Unreal 서식에 맞게 만들기 위함.
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
	* Request는 요청만, HostSession은 실제로 수행!
	*/
	ULocalPlayer* LocalPlayer = (HostingPlayer != nullptr) ? HostingPlayer->GetLocalPlayer() : nullptr;
	if (!LocalPlayer)
	{
		return;
	}

	/*
	* ClinetTravel -> 다른 Dedicated 서버로 이동
	* ServerTravel -> 해당서버 Client전체 인동
	*/
	GetWorld()->ServerTravel(Request->ConstructTravelURL());


}
