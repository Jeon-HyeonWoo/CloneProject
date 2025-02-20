// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneGameModeBase.h"

#include "CloneGameState.h"
#include "CloneProject/Player/ClonePlayerController.h"
#include "CloneProject/Player/ClonePlayerState.h"
#include "CloneProject/Character/CloneCharacter.h"
#include "CloneProject/GameModes/CloneExperienceManagerComponent.h"
#include "CloneProject/GameModes/CloneExperienceDefinition.h"
#include "CloneProject/CloneLogChannels.h"
#include "CloneProject/Character/ClonePawnData.h"
#include "CloneProject/Character/ClonePawnExtensionComponent.h"


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

UClass* ACloneGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UClonePawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	//return DefaultPawnClass::StaticClass();
	return Super::GetDefaultPawnClassForController_Implementation(InController);
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
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UClonePawnExtensionComponent* PawnExtComp = UClonePawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UClonePawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}


	UE_LOG(LogClone, Log, TEXT("SpawnDefaultPawnAtTransform_Implementation is called")); //Spawn Pawn
	return nullptr;
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
	//World�� �ִ� ��� PlayerController�� ��ȸ�ϸ� ReStart�� ���ش�. 
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				/*
					ReStartPlayer : FindPlayerStart(StartSpot) -> SpawnDefaultPawn -> SetPawn(DefaultPawn)
					Experience�� ������ �� �� ����, GameMode���� ���Ƶ� FindSpot, PawnSetting, Spawn���� 
					Loaded�� �Ǿ����� �ٽ� �����Ͽ� �����ص� DefualtPawn�� ��ȯ�ϴ� ��.
				*/
				RestartPlayer(PC);
			}
		}
	}
}

const UClonePawnData* ACloneGameModeBase::GetPawnDataForController(const AController* InController) const
{
	//PlayerState�� �̹� PawnData�� �����Ѵٸ� PawnData�� Return.
	if (InController)
	{
		if (const AClonePlayerState* ClonePS = InController->GetPlayerState<AClonePlayerState>())
		{
			if (const UClonePawnData* PawnData = ClonePS->GetPawnData<UClonePawnData>())
			{
				return PawnData;
			}
		}
	}
	
	check(GameState);

	UCloneExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UCloneExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		const UCloneExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}

	return nullptr;
}
