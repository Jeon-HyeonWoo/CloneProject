// Fill out your copyright notice in the Description page of Project Settings.


#include "ClonePlayerState.h"

#include "CloneProject/GameModes/CloneGameModeBase.h"
#include "CloneProject/GameModes/CloneExperienceManagerComponent.h"

/*
	PlayerState는, Charcter, PlayerController 생성 이후에 생성 되기 때문에 GameState는 생성이 되어있다.
*/
void AClonePlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	const AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);

	UCloneExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UCloneExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnCloneExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));

}

void AClonePlayerState::OnExperienceLoaded(const UCloneExperienceDefinition* CurrentExperience)
{
	/*
	* PawnData는 CurrentExperience에 있어서 Manager에서 가져와야하는데, 
	* CurrentExperience에 PawnData가 있음에도 GameMode에서 가져오는 이유는? 
	* 1. 순수 안전성 체크
	* 2. PawnData를 가져오는 것을 하나의 함수로 처리하기 위함.
	*	- PlayerState와, GameMode 둘 다 사용할 것이기 때문에.
	*/	
	if (ACloneGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ACloneGameModeBase>())
	{
		const UClonePawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check(NewPawnData);

		SetPawnData(NewPawnData);
	}
}

void AClonePlayerState::SetPawnData(const UClonePawnData* InPawnData)
{
	check(InPawnData);

	//중복 설정 방지
	check(!PawnData);

	PawnData = InPawnData;
}
