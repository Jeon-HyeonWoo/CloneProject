// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ClonePlayerState.generated.h"

/**
 * 
 */

class UCloneExperienceDefinition;
class UClonePawnData;

UCLASS()
class CLONEPROJECT_API AClonePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	virtual void PostInitializeComponents() final;

	void OnExperienceLoaded(const UCloneExperienceDefinition* CurrentExperience);

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void SetPawnData(const UClonePawnData* InPawnData);

	UPROPERTY()
	TObjectPtr<const UClonePawnData> PawnData;
};
/*
	Experience Loading�� �Ϸᰡ �Ǿ� �ִٸ�,
	Definition�� PawnData�� �����Ű�, Experience�� CurrentExperience�� Data�� �� �ִ� �� �ƴѰ�?
	�� State�� ������ �ΰ� ���� Casing�ϴ� ������ �����ΰ�?
	: ���� PlayerState�� AbilitySystemComponent���� �װ��� PlayerState�� �����.
	�� AbilitySystem�� PawnData�� �����ؼ� �ɷ��� �ο��� ���̱� ������ �̸� Casing�� �س��´�.
*/

/*
	
*/