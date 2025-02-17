// Fill out your copyright notice in the Description page of Project Settings.


#include "ClonePlayerState.h"

#include "CloneProject/GameModes/CloneGameModeBase.h"
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
	/*
	* PawnData�� CurrentExperience�� �־ Manager���� �����;��ϴµ�, 
	* CurrentExperience�� PawnData�� �������� GameMode���� �������� ������? 
	* 1. ���� ������ üũ
	* 2. PawnData�� �������� ���� �ϳ��� �Լ��� ó���ϱ� ����.
	*	- PlayerState��, GameMode �� �� ����� ���̱� ������.
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

	//�ߺ� ���� ����
	check(!PawnData);

	PawnData = InPawnData;
}
