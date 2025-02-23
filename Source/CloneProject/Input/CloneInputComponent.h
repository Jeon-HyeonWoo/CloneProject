// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "InputActionValue.h"
#include "CloneInputConfig.h"
#include "EnhancedInputComponent.h"

#include "CloneInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class CLONEPROJECT_API UCloneInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:

	UCloneInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	template<class UserClass, typename FuncType>
	void BindNatvieAction(
		const UCloneInputConfig* InputConfig, 
		const FGameplayTag& InputTag, 
		ETriggerEvent TriggerEvent, 
		UserClass* Object, 
		FuncType Func, 
		bool bLogIfNotFound
	);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(
		const UCloneInputConfig* InputConfig,
		UserClass* Object,
		PressedFuncType PressedFunc,
		ReleasedFuncType ReleasedFunc,
		TArray<int32>& BindHandles
	);

};

template<class UserClass, typename FuncType>
void UCloneInputComponent::BindNatvieAction(const UCloneInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);

	if (const UInputAction* IA = InputConfig->FindNatvieInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UCloneInputComponent::BindAbilityActions(const UCloneInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<int32>& BindHandles)
{
}
