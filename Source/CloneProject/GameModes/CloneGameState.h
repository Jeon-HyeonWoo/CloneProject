// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CloneGameState.generated.h"


class UCloneExperienceManagerComponent;
/**
 * 
 */
UCLASS()
class CLONEPROJECT_API ACloneGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	ACloneGameState();

public:

	UPROPERTY()
	TObjectPtr<UCloneExperienceManagerComponent> ExperienceManagerComponent;

};
