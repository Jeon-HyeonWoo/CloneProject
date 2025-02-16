// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneGameModeBase.h"

#include "CloneGameState.h"
#include "CloneProject/Player/ClonePlayerController.h"
#include "CloneProject/Player/ClonePlayerState.h"
#include "CloneProject/Character/CloneCharacter.h"
#include "CloneProject/GameModes/CloneExperienceManagerComponent.h"
#include "CloneProject/CloneLogChannels.h"

ACloneGameModeBase::ACloneGameModeBase()
{
	//Defualt GameMode Settings..
	GameStateClass = ACloneGameState::StaticClass();
	PlayerControllerClass = AClonePlayerController::StaticClass();
	PlayerStateClass = AClonePlayerState::StaticClass();
	DefaultPawnClass = ACloneCharacter::StaticClass();
}

/*
*	Experience Loading.
*/
void ACloneGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	/*
		work to NextFrame
	*/
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ACloneGameModeBase::InitGameState()
{
	/* 
		GameState가 생성이 된 이후에 호출되는 함수.
		GameState가 만들어졌다는 것은, GameState에서 ExperienceManagerComponent를 만들었다는 뜻이고,
		Delegate를 등록할 수 있다는 뜻.
	*/
	Super::InitGameState();

	// Experience 비동기 로딩을 위한 Delegate
	UCloneExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UCloneExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	//OnExperienceLoaded 등록
	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnCloneExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void ACloneGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	/*
		HandleStartingNewPlayer -> RestartPlayerAtStart -> SpawnDefaultPawnFor -> SpawnDefaultPawnAtTransform 
		-> GetDefaultPawnClassForController
		위 순서도와 같이 진행이 되었을 때, Experience가 생성되지 않았음으로, 해당 HandleStartingNewPlayer 함수를 
		Experience가 생성되기 전까지 막아두면, Experience가 생성된 이후 실행되도록 함.
		* 왜 Experience가 생성되고 나서야 Player를 탐색하냐면, ExperienceDefinitation에 PawnData를 받은 Character를 소환해야하기 때문이다.
	*/
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

APawn* ACloneGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	UE_LOG(LogClone, Log, TEXT("SpawnDefaultPawnAtTransform_Implementation is called")); //Spawn Pawn
	return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
	
}

/*
	역할 : Experience에게 Load할 Experience정보를 보내고 Loading 유도.
	Handle Match, If Not Expectiong 인 이유는 무조건적인 Experience Loading이 아닌,
	Handle과 매칭해서 알맞은 Experience와 매칭하기 위해서.
*/
/*
*/
void ACloneGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;

	UWorld* World = GetWorld();

	//Default옵션으로 BP_CloneDefaultExperience로 강제 설정, Scan이 가능하도록 Id 설정/ Type, Name
	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("CloneExperienceDefinition"), FName("BP_CloneDefaultExperience"));
	}

	OnMatchAssignmentGiven(ExperienceId);
}

void ACloneGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	check(ExperienceId.IsValid());

	UCloneExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UCloneExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	//Lyra프로젝트에서  Network상에서는 GameMode가 cline가 아닌 server에 존재하기 때문에 함수 명칭이 이러하다.
	ExperienceManagerComponent->ServerSetCurrentExperience(ExperienceId);
}

bool ACloneGameModeBase::IsExperienceLoaded() const
{
	check(GameState);
	UCloneExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UCloneExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	
	return ExperienceManagerComponent->IsExperienceLoaded();
}

void ACloneGameModeBase::OnExperienceLoaded(const UCloneExperienceDefinition* CurrentExperience)
{
}
