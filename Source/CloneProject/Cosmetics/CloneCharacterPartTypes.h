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

	/* ������ ������ Actor�� class ������ ������ ���� -> Actor BP�� �ϳ��� ������ �����ϸ� �ȴ�. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PartClass;

	/* Actor�� ��ȯ�Ͽ� ���̴µ� Ư�� Bone�� ���̱� ���� ���� (�ٸ� ���ǰ� ���ٸ� Root�� ����) */
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