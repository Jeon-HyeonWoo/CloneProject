// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CloneMappableConfigPair.generated.h"

/*
*	forward declaration
*/
class UPlayerMappableInputConfig;

/**
 * 
 */

USTRUCT()
struct FCloneMappableConfigPair
{
	GENERATED_BODY()

public:

	FCloneMappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;
};