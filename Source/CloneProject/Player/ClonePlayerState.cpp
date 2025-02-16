// Fill out your copyright notice in the Description page of Project Settings.


#include "ClonePlayerState.h"
#include "CloneProject/GameModes/CloneExperienceManagerComponent.h"

/*
	PlayerState��, Charcter, PlayerController ���� ���Ŀ� ���� �Ǳ� ������ GameState�� ������ �Ǿ��ִ�.
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
