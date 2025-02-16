// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneGameState.h"
#include "CloneProject/GameModes/CloneExperienceManagerComponent.h"

ACloneGameState::ACloneGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UCloneExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
