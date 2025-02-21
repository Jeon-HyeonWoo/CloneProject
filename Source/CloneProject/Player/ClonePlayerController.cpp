// Fill out your copyright notice in the Description page of Project Settings.


#include "ClonePlayerController.h"
#include "CloneProject/Camera/ClonePlayerCameraManager.h"

AClonePlayerController::AClonePlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	/*
	*	GameMode에서 DefaultPawnClass나, DefaultState를 지정하듯, PlayerController에서도 CameraManager를 Defualt값으로 지정가능.
	*/
	PlayerCameraManagerClass = AClonePlayerCameraManager::StaticClass();
}

