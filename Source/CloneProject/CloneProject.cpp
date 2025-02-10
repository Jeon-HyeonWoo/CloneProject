// Copyright Epic Games, Inc. All Rights Reserved.

#include "CloneProject.h"
#include "CloneLogChannels.h"
#include "Modules/ModuleManager.h"

class FCloneProjectModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FCloneProjectModule::StartupModule()
{
	UE_LOG(LogClone, Warning, TEXT("Startup Module"));
}

void FCloneProjectModule::ShutdownModule()
{

}

IMPLEMENT_PRIMARY_GAME_MODULE(FCloneProjectModule, CloneProject, "CloneProject" );
