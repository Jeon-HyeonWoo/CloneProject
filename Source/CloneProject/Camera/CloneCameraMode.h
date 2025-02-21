// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CloneCameraMode.generated.h"

/*
*	Lyra�� �ñ��� ��ǥ : ���ȭ.
*	ex) Mode_FPS, Mode_InCar, Mode_RPG..

	CameraModeStack : CameraMode�� Stackȭ
	Stackȭ�� ���� : Camera�� �ڿ������� ��ȯ�� ���� Mode���� �� ����
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
	//������ CameraMode ����
	UPROPERTY() 
	TArray<TObjectPtr<UCloneCameraMode>> CameraModeInstance;

	//Camera Matrix Blend ������Ʈ
	UPROPERTY()
	TArray<TObjectPtr<UCloneCameraMode>> CameraModeStack;
};

