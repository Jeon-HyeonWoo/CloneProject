// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FShooterCoreRuntimeModule : public IModuleInterface
{
public:
	//~IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~End of IModuleInterface
};
/*	
*	
*/
/*
*	DA_ShooterCore
* 
*	DA's State 
*	1. Installed : ProjectSettings에 None Check
*	2. Registered : PlugIn Install Check, Store Caching
*	3. Loaded : In Memory.
*	4. Active : 
*	#. DeActivate : Shut Down Active
* 
*	Actions
*	- RunTime단계에서 Pawn에게 부여하는 행동을 나타냄
* 
*	Primary Asset Types Scan 
*	- ProjectSettings에 있는 AssetManager는 Unreal Content에 사용되는 것,
*	- 따로 Features의 Content에서 사용하도록 AssetManager를 설정
*/