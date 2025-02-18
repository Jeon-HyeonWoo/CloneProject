// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ClonePawnData.generated.h"

/**
 * 
 */
UCLASS()
class CLONEPROJECT_API UClonePawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	

public:

	UClonePawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clone|Pawn")
	TSubclassOf<APawn> PawnClass;
};
