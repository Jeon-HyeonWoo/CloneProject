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
		GameState�� ������ �� ���Ŀ� ȣ��Ǵ� �Լ�.
		GameState�� ��������ٴ� ����, GameState���� ExperienceManagerComponent�� ������ٴ� ���̰�,
		Delegate�� ����� �� �ִٴ� ��.
	*/
	Super::InitGameState();

	// Experience �񵿱� �ε��� ���� Delegate
	UCloneExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UCloneExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	//OnExperienceLoaded ���
	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnCloneExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void ACloneGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	/*
		HandleStartingNewPlayer -> RestartPlayerAtStart -> SpawnDefaultPawnFor -> SpawnDefaultPawnAtTransform 
		-> GetDefaultPawnClassForController
		�� �������� ���� ������ �Ǿ��� ��, Experience�� �������� �ʾ�������, �ش� HandleStartingNewPlayer �Լ��� 
		Experience�� �����Ǳ� ������ ���Ƶθ�, Experience�� ������ ���� ����ǵ��� ��.
		* �� Experience�� �����ǰ� ������ Player�� Ž���ϳĸ�, ExperienceDefinitation�� PawnData�� ���� Character�� ��ȯ�ؾ��ϱ� �����̴�.
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
	���� : Experience���� Load�� Experience������ ������ Loading ����.
	Handle Match, If Not Expectiong �� ������ ���������� Experience Loading�� �ƴ�,
	Handle�� ��Ī�ؼ� �˸��� Experience�� ��Ī�ϱ� ���ؼ�.
*/
/*
*/
void ACloneGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;

	UWorld* World = GetWorld();

	//Default�ɼ����� BP_CloneDefaultExperience�� ���� ����, Scan�� �����ϵ��� Id ����/ Type, Name
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

	//Lyra������Ʈ����  Network�󿡼��� GameMode�� cline�� �ƴ� server�� �����ϱ� ������ �Լ� ��Ī�� �̷��ϴ�.
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
