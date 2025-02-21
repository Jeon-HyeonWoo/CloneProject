// Fill out your copyright notice in the Description page of Project Settings.


#include "ClonePlayerController.h"
#include "CloneProject/Camera/ClonePlayerCameraManager.h"

AClonePlayerController::AClonePlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	/*
	*	GameMode���� DefaultPawnClass��, DefaultState�� �����ϵ�, PlayerController������ CameraManager�� Defualt������ ��������.
	*/
	PlayerCameraManagerClass = AClonePlayerCameraManager::StaticClass();
}

