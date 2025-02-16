// Fill out your copyright notice in the Description page of Project Settings.


#include "ClonePlayerState.h"
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

}
