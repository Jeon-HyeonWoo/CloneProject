// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneInputConfig.h"
#include "CloneProject/CloneLogChannels.h"

#include "InputAction.h"

UCloneInputConfig::UCloneInputConfig(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

const UInputAction* UCloneInputConfig::FindNatvieInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const FCloneInputAction& Action : NatvieInputActions)
    {
        if (Action.InputAction && (Action.InputTag == InputTag))
        {
            return Action.InputAction;
        }
   }

    if (bLogNotFound)
    {
        UE_LOG(LogClone, Error, TEXT("can not find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe((this)));
    }

    return nullptr;
}

const UInputAction* UCloneInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const FCloneInputAction& Action : AbilityInputActions)
    {
        if (Action.InputAction && (Action.InputTag == InputTag))
        {
            return Action.InputAction;
        }
    }

    if (bLogNotFound)
    {
        UE_LOG(LogClone, Error, TEXT("can not find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe((this)));
    }

    return nullptr;
}
