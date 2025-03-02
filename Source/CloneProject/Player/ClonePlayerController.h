// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerController.h"
#include "ClonePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CLONEPROJECT_API AClonePlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:

	AClonePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


};
