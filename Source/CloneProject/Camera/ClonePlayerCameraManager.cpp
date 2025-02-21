// Fill out your copyright notice in the Description page of Project Settings.


#include "ClonePlayerCameraManager.h"

AClonePlayerCameraManager::AClonePlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DefaultFOV = CLONE_CAMERA_DEFAULT_FOV;			//시야 각도
	ViewPitchMin = CLONE_CAMERA_DEFAULT_PITCH_MIN;	//위
	ViewPitchMax = CLONE_CAMERA_DEFAULT_PITCH_MAX;	//아래
}
