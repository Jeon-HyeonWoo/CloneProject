// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonSessionSubSystem.generated.h"

/**
 * HostSession�� ����� Subsystem�� �����ϸ�,
 * Subsystem���� MapID�� CmdArgs�� �������� CmdArgs�� �����.
 */
UCLASS(BlueprintType)
class COMMONUSER_API UCommonSession_HostSessionRequest : public UObject
{
	GENERATED_BODY()
public:

	/* MapID -> TEXT�� ��ȯ */
	FString GetMapName() const;

	/* serverTravel�� ������ ���� URL ���� */
	FString ConstructTravelURL() const;

	/* �غ��� MapID (�� ���) */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	FPrimaryAssetId MapID;

	/* ������ CmdArgs (Experience �̸��� ������) -> �� UserFacingExperienceDefinition���� Ȯ�� */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	TMap<FString, FString> ExtraArgs;
};

/*
*	Mecro�� ClassName �� API�� ���� �ʴ´ٸ�, �ش� class�� ���� Plugin �ܿ� �� �� ����.
*	��ġ Plugin(Module)�� Protected: ���� ����?..
*	�����δ� ������ �� ���ٰ� ���.
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
	SubSystem�̶�?

	1. ���� : EngineSub, EditorSub, GameInstSub, WorldSub, LoaclPlayerSub
	2. ���� : SingleTon(���� : ������, ��𼭵� ����) (���� : Unreal���� �� �� ������ ���� ������� ����)
	3. ������� : Unreal���� Singleton�� �����ֱ�� ���� ������ ���� (ver. 4.2~)
	4. �� Name_Subsystem�� Name�����ֱ⿡ ���� ���� �� �ı� -> ������ �����ֱ⸦ ���� Engine�� ȿ����
	5. ���Ǽ� : UMySystem* MySystem = GetSubsystem<UMySystem>(); // �ҷ����� ����
	6. ���� : ShouldCreate(��������), Init(�ʱ�ȭ), Deinit(����)

	GameInstnace�� ������ �������� �Ȼ�������� ���� �� Subsystem����.
*/