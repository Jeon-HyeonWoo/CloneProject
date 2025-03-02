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
	
	/* AnimLayer�� ������ ��� SkeletalMesh */
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

	/* GameplayTag�� ���� (CosmeticTags), MeshRules�� ���� �˸��� BodyStyle�� ��ȭ�Ѵ�. */
	//USkeletalMesh* SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const;

	/* AnimLayer ������ SkeletalMesh�� ��� ���� -> Animation-mesh�� Rules�� MeshRules��� ����
	*  = USkeletalMesh + GameplayTag
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCloneAnimBodyStyleSelectionEntry> MeshRules;

	/* Just Default Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;

	/* Physics Asset�� �ϳ��� ���� -> ��� Animation�� Physics�Ӽ� ���� */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicsAsset> ForcedPhysicsAsset = nullptr;
};