// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CloneCosmeticAnimationTypes.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FCloneAnimBodyStyleSelectionEntry
{
	GENERATED_BODY()
	
	/* AnimLayer를 적용할 대상 SkeletalMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> Mesh = nullptr;

	/* Cosmetic Tag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Cosmetic"))
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FCloneAnimBodyStyleSelectionSet
{
	GENERATED_BODY()

	/* GameplayTag를 통해 (CosmeticTags), MeshRules에 따라 알맞은 BodyStyle을 변화한다. */
	//USkeletalMesh* SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const;

	/* AnimLayer 적용할 SkeletalMesh를 들고 있음 -> Animation-mesh간 Rules을 MeshRules라고 생각
	*  = USkeletalMesh + GameplayTag
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCloneAnimBodyStyleSelectionEntry> MeshRules;

	/* Just Default Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;

	/* Physics Asset은 하나로 동일 -> 모든 Animation의 Physics속성 공유 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicsAsset> ForcedPhysicsAsset = nullptr;
};