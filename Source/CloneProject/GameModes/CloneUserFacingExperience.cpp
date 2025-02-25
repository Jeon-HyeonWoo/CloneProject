// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneUserFacingExperience.h"
#include "CommonSessionSubSystem.h"

UCommonSession_HostSessionRequest* UCloneUserFacingExperience::CreateHostingRequest() const
{
    //hope to move experience name
    const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();

    UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();

    Result->MapID = MapID;
    Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);

    return Result;
}
