// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ClonePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CLONEPROJECT_API AClonePlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AClonePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


};
