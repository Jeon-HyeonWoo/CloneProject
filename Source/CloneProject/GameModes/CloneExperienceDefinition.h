// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CloneExperienceDefinition.generated.h"

class UClonePawnData;
/**
 * 
 */
UCLASS()
class CLONEPROJECT_API UCloneExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UCloneExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<UClonePawnData> DefaultPawnData;


	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;
};
