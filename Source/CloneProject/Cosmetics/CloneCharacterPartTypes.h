// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CloneCharacterPartTypes.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FCloneCharacterPartHandle
{
	GENERATED_BODY()

	void Reset()
	{
		PartHandle = INDEX_NONE;
	}

	bool IsValid() const
	{
		return PartHandle != INDEX_NONE;
	}

	UPROPERTY()
	int32 PartHandle = INDEX_NONE;

};

USTRUCT(BlueprintType)
struct FCloneCharacterPart
{
	GENERATED_BODY()

	/* 파츠로 생성된 Actor의 class 정보를 가지고 있음 -> Actor BP를 하나의 파츠로 생각하면 된다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PartClass;

	/* Actor를 소환하여 붙이는데 특정 Bone에 붙이기 위한 정보 (다른 정의가 없다면 Root에 부착) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;
};

/*
*	Type - CloneCharacterPart : TSubclassof<AActor>, FName
*		 - CloneCharacterPartHandle : int32
* 
*	ControllerPart - CloneCharacterPartEntry : CloneCharacterPart + PartHandle
*				   
*	PawnpPart - CloneAppliedCharacterPartEntry : CloneCharacterPart + TobjectPtr<Child>
*			  - CloneCharacterPartList : AppliedChracterPartEntry + OwnerComponent
*/