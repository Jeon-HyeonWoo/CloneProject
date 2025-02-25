// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonSessionSubSystem.generated.h"

/**
 * HostSession을 만들어 Subsystem에 전달하면,
 * Subsystem에서 MapID와 CmdArgs로 최종적인 CmdArgs를 만든다.
 */
UCLASS(BlueprintType)
class COMMONUSER_API UCommonSession_HostSessionRequest : public UObject
{
	GENERATED_BODY()
public:

	/* MapID -> TEXT로 변환 */
	FString GetMapName() const;

	/* serverTravel에 전달할 최종 URL 생성 */
	FString ConstructTravelURL() const;

	/* 준비할 MapID (맵 경로) */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	FPrimaryAssetId MapID;

	/* 전달할 CmdArgs (Experience 이름을 전달함) -> 곧 UserFacingExperienceDefinition에서 확인 */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	TMap<FString, FString> ExtraArgs;
};

/*
*	Mecro인 ClassName 앞 API를 쓰지 않는다면, 해당 class를 같은 Plugin 외에 쓸 수 없다.
*	마치 Plugin(Module)의 Protected: 같은 느낌?..
*	오류로는 참조할 수 없다고 뜬다.
*/
UCLASS()
class COMMONUSER_API UCommonSessionSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UCommonSessionSubSystem() { }

	UFUNCTION(BlueprintCallable, Category = Session)
	void HostSession(APlayerController* HostingPlayer, UCommonSession_HostSessionRequest* Request);

	FString PendingTravelURL;
};

/*
	SubSystem이란?

	1. 종류 : EngineSub, EditorSub, GameInstSub, WorldSub, LoaclPlayerSub
	2. 형태 : SingleTon(장점 : 고유성, 어디서든 접근) (단점 : Unreal에서 한 번 생성시 절대 사라지지 않음)
	3. 사용이유 : Unreal에서 Singleton의 생명주기로 인한 단점을 보완 (ver. 4.2~)
	4. 각 Name_Subsystem의 Name생명주기에 따라 생성 및 파괴 -> 복잡한 생명주기를 가진 Engine에 효율적
	5. 편의성 : UMySystem* MySystem = GetSubsystem<UMySystem>(); // 불러오기 편함
	6. 지원 : ShouldCreate(생성막기), Init(초기화), Deinit(제거)

	GameInstnace는 게임을 끌때까지 안사라짐으로 게임 시 Subsystem유지.
*/