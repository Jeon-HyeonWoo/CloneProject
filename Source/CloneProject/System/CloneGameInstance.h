// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CloneGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CLONEPROJECT_API UCloneGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	/* GameInstance's Interface */
	virtual void Init() override;
	virtual void Shutdown() override;
};
