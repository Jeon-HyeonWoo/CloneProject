// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CloneGameModeBase.generated.h"

/**
 * 
 */

class UCloneExperienceDefinition;

UCLASS()
class CLONEPROJECT_API ACloneGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ACloneGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() final;

	//Implementation�� ���� �� Blueprint Only, �ִ°� C++����
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;

	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;

	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const UCloneExperienceDefinition* CurrentExperience);
};

/*
	GameMode 
	+ GameStateClass : GameMode�� 1��1 �������� 1���� ���� StateClass�� 1�� 
		- has ExperienceComponent : ExperienceComponentManager�� �ε��� ����.
		- Experience�� Unreal�� ������ �ƴϱ� ������, Experience�� �ε��� �Ϸ� �� �ʱ�ȭ ����

	+ PlayerStateClass
		- PawnData : � Pawn�� �� ���ΰ�?, � InputConfig�� �� ���ΰ�?, CameraMode�� ��� �� ���ΰ�?
		- CameraComponent�� Character�� �����ϴµ� ��°�� PawnData���� ���°�? : 1��Ī, TopDown��� ���� ī�޶� ��带 �����Ͽ�, Character�� ����

	+ PlayerControllerClass
		- PlayerCameraManager : 

	+ DefaultPawnClass
		- PawnExtensionComponent : Character�� ���� Extension(����)�� ������ �ִ� Ŭ����
		- CameraComponent : �ʰ� �� �˰� �ִ� �� ī�޶� ������Ʈ, Player�� ���� �ִ� Camera�� ȭ���� PlayerController_PlayerCameraManager���� ������ ó��
		Why? : Character�� ������ �Ͼ�� ������
*/

/*
*	GameMode�� ExperienceDefinition�� ������ Loading�ϴ� ����
* 
	1. GameModeBase���� InitGame�� NextTick�� HandleMatchAssignmentIfNotExpectingOne ��û

	2. HandleMatch���� PrimaryAssetId�� �⺻������ ����(Type : ExperienceDefinition, Name : BP_DefaultExperience)

	3. OnMatchAssignmentGiven���� ExperienceManagerComponent�� �̿��Ͽ�(SetNextFrame�� ��� ������ Manager�� ������ �Ǿ��ִ�.)
		ServerSetCurrentExperience�� ȣ��.

	4. ServerSetCurrentExperience���� AssetManager�� ȣ��. 
		AssetManager���� �ش� Definition�� Scan�� �ȴٸ�,
		Path�� �޾Ƽ�(FSoftObjectPath),
		�ش� ��θ� TryLoad()�Ͽ� TSubclassOf�� ����

	5. TSubClassOf�� Class Default Object�� �����´�.

	6. �ش� CDO�� Manager�� �ִ� CurrentExperience�� �����Ѵ�.

	7. CurrentExperience�� ���� �ִ��� Loading�� �� ���� �ƴ�, StartExperienceLoad�� ���� �ε��� �Ѵ�.
	//TryLoad�� �������� �ұ��ϰ� StartExperience�� load�ϴ� ����, BundleList�� �̿��� �뷮�� Experience�� ������ �з��ϱ� ����

	8. StartExperience���� BundleAsset, BundleToLoad�� ���� Server�� Client�� ������.

	9. �з��� �� Bundle�� �� �� �� load�Ѵ�.

	10. �з��� Bundle�� Loaded�� �ȴٸ�, OnExperienceLoadComplete�� ȣ��

	11. ���� Loading�� �Ϸ�� CurrentExperience�� BroadCast�� �����༭ GameMode�� PlayerState�� ������ �����༭ �ʱ�ȭ�� ����ȴ�.
*/