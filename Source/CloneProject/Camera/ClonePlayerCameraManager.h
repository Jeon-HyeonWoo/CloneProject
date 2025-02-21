// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ClonePlayerCameraManager.generated.h"


/*
	Manager System�� ���ϼ��� �����ϰ� �װ��� Engine Code�� overrdie������,
	PlayerCameraManager�� PlayerController�� �����Ѵ�.
*/
/**
 * Controller�� Binding �� CameraManager
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
