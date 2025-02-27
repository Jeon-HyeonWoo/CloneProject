// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CloneExperienceActionSet.generated.h"

class UGameFeatureAction;

/**
 * 
 */
UCLASS()
class CLONEPROJECT_API UCloneExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UCloneExperienceActionSet();

	/* member variables */
	UPROPERTY(EditAnywhere, Category = "Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;


};
