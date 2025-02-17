// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CloneGameModeBase.generated.h"

/**
*  정리
* 1. GameMode생성 이후 InitGame내부에서 다음 프레임(SetNextFrame)에 HandleMatchAssignmentIfNotExpectingOne을 실행한다.
* 2. InitGameState에서 CallorRegistr_OnExperienceLoaded에서 ExperienceManager를 통해 
*		Experience가 Loading이 완료가 된다면, OnExperienceLoaded를 호출하도록 Delegate를 설정.
* 3. ExperienceMangerComponent가 생성된 시점 
* 4. HandleMatchAssignmentIfNotExpectingOne 실행 (Default Experience 설정) 및 Given실행
* 5. OnMatchAssignmentGiven에서는 PrimaryId(ExperienceId)를 가지고 SetCurrentExperience실행
* 6. SetCurrentExperience에서는 Id를 가지고 경로를 받아온다음 Experience를 TryLoad 
* 7. CDO를 가져와서 분류한 후 다시 Load
* 8. Load가 완료가 된다면, FullLoadComplete에서 Delegate를 BroadCast.
* 9. OnExperienceLoaded가 BroadCasting되면 불려지면서,  Pawn, and Controller 설정. PlayerState는 해당 Pawn, Controller를 Casing
*/

class UCloneExperienceDefinition;
class UClonePawnData;

UCLASS()
class CLONEPROJECT_API ACloneGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ACloneGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() final;

	/** GetDefualtPawnClassForController **/
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) final;

	//Implementation이 없는 건 Blueprint Only, 있는건 C++영역
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;

	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;

	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const UCloneExperienceDefinition* CurrentExperience);
	const UClonePawnData* GetPawnDataForController(const AController* InController) const;
};

/*
	GameMode 
	+ GameStateClass : GameMode와 1대1 대응으로 1게임 모드당 StateClass는 1개 
		- has ExperienceComponent : ExperienceComponentManager로 로딩을 진행.
		- Experience는 Unreal의 개념이 아니기 때문에, Experience의 로딩이 완료 후 초기화 진행

	+ PlayerStateClass
		- PawnData : 어떤 Pawn을 쓸 것인가?, 어떤 InputConfig를 쓸 것인가?, CameraMode는 어떻게 할 것인가?
		- CameraComponent는 Character에 부착하는데 어째서 PawnData에서 쓰는가? : 1인칭, TopDown등등 많은 카메라 모드를 장착하여, Character에 전송

	+ PlayerControllerClass
		- PlayerCameraManager : 

	+ DefaultPawnClass
		- PawnExtensionComponent : Character에 대한 Extension(정보)를 가지고 있는 클래스
		- CameraComponent : 너가 잘 알고 있는 그 카메라 컴포넌트, Player가 보고 있는 Camera의 화면을 PlayerController_PlayerCameraManager에게 보내서 처리
		Why? : Character의 변형이 일어나도 ㄱㅊ게
*/

/*
*	GameMode의 ExperienceDefinition의 정보를 Loading하는 과정
* 
	1. GameModeBase에서 InitGame에 NextTick에 HandleMatchAssignmentIfNotExpectingOne 요청

	2. HandleMatch에서 PrimaryAssetId를 기본값으로 설정(Type : ExperienceDefinition, Name : BP_DefaultExperience)

	3. OnMatchAssignmentGiven에서 ExperienceManagerComponent를 이용하여(SetNextFrame을 썼기 때문에 Manager는 생성이 되어있다.)
		ServerSetCurrentExperience를 호출.

	4. ServerSetCurrentExperience에서 AssetManager를 호출. 
		AssetManager에서 해당 Definition이 Scan이 된다면,
		Path를 받아서(FSoftObjectPath),
		해당 경로를 TryLoad()하여 TSubclassOf로 받음

	5. TSubClassOf를 Class Default Object로 가져온다.

	6. 해당 CDO를 Manager에 있는 CurrentExperience에 저장한다.

	7. CurrentExperience에 값이 있더라도 Loading이 된 것이 아닌, StartExperienceLoad를 통해 로딩을 한다.
	//TryLoad를 했음에도 불구하고 StartExperience르 load하는 것은, BundleList를 이용해 대량의 Experience의 정보를 분류하기 위함

	8. StartExperience에서 BundleAsset, BundleToLoad를 통해 Server와 Client를 구분함.

	9. 분류가 된 Bundle을 한 번 더 load한다.

	10. 분류된 Bundle이 Loaded가 된다면, OnExperienceLoadComplete를 호출

	11. 현재 Loading이 완료된 CurrentExperience를 BroadCast로 보내줘서 GameMode의 PlayerState에 정보를 보내줘서 초기화가 진행된다.

	12. Loading이 된 Experience의 정보를 통해 GameMode에서 막아두었던 GameModeSetting에 필요한 것들을 Loading하기 시작한다.
	* Experience에서 
*/