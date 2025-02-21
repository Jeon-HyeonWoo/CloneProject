// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CloneCameraMode.generated.h"

/*
*	Lyra의 궁극적 목표 : 모듈화.
*	ex) Mode_FPS, Mode_InCar, Mode_RPG..

	CameraModeStack : CameraMode를 Stack화
	Stack화의 이유 : Camera의 자연스러운 전환을 위해 Mode변경 시 보간
*/
UCLASS()
class CLONEPROJECT_API UCloneCameraMode : public UObject
{
	GENERATED_BODY()
	
public:

	UCloneCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};


UCLASS()
class UCloneCameraModeStack : public UObject
{
	GENERATED_BODY()

public:

	UCloneCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* member variables */
	//생성된 CameraMode 관리
	UPROPERTY() 
	TArray<TObjectPtr<UCloneCameraMode>> CameraModeInstance;

	//Camera Matrix Blend 업데이트
	UPROPERTY()
	TArray<TObjectPtr<UCloneCameraMode>> CameraModeStack;
};

