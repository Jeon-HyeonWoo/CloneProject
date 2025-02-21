// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ClonePlayerCameraManager.generated.h"


/*
	Manager System은 단일성을 보장하고 그것을 Engine Code에 overrdie했지만,
	PlayerCameraManager는 PlayerController가 관리한다.
*/
/**
 * Controller에 Binding 된 CameraManager
 */
#define CLONE_CAMERA_DEFAULT_FOV (80.0f)
#define CLONE_CAMERA_DEFAULT_PITCH_MIN (-89.0f)
#define CLONE_CAMERA_DEFAULT_PITCH_MAX (89.0f) 


UCLASS()
class CLONEPROJECT_API AClonePlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	AClonePlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	
};
