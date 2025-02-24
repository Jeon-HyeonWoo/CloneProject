// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CloneInputConfig.generated.h"

class UInputAction;

/*
* 
*/

USTRUCT(BlueprintType)
struct FCloneInputAction
{
	GENERATED_BODY()
	/*
	* �ܼ��� InputAction* �ϰ� Tag�ϰ� Mapping�س��� �뵵
	*/
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * 
 */
UCLASS(Blueprintable)
class CLONEPROJECT_API UCloneInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UCloneInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UInputAction* FindNatvieInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	/*
		member  variable
	*/

	/* 
		�������� Input
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputAction"))
	TArray<FCloneInputAction> NatvieInputActions;

	/*
		GameAbilitySystem's Input.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputAction"))
	TArray<FCloneInputAction> AbilityInputActions;
	
};
