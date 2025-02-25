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
*	1. Installed : ProjectSettings�� None Check
*	2. Registered : PlugIn Install Check, Store Caching
*	3. Loaded : In Memory.
*	4. Active : 
*	#. DeActivate : Shut Down Active
* 
*	Actions
*	- RunTime�ܰ迡�� Pawn���� �ο��ϴ� �ൿ�� ��Ÿ��
* 
*	Primary Asset Types Scan 
*	- ProjectSettings�� �ִ� AssetManager�� Unreal Content�� ���Ǵ� ��,
*	- ���� Features�� Content���� ����ϵ��� AssetManager�� ����
*/