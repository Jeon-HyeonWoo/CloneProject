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
	Experience Loading이 완료가 되어 있다면,
	Definition에 PawnData가 있을거고, Experience에 CurrentExperience에 Data가 다 있는 것 아닌가?
	왜 State에 변수를 두고 따로 Casing하는 이유가 무엇인가?
	: 추후 PlayerState에 AbilitySystemComponent부착 그것을 PlayerState에 만들것.
	그 AbilitySystem을 PawnData를 참고해서 능력을 부여할 것이기 때문에 미리 Casing을 해놓는다.
*/

/*
	
*/