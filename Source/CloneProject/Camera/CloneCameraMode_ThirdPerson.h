// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CloneCameraMode.h"
#include "CloneCameraMode_ThirdPerson.generated.h"

class UCurveVector;
/**
 * 
 */
//Abstract(�߻�) : �� ���̶� ��� �޾ƾ� ��밡��.
UCLASS(Abstract, Blueprintable)
class CLONEPROJECT_API UCloneCameraMode_ThirdPerson : public UCloneCameraMode
{
	GENERATED_BODY()
	
public:

	UCloneCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/*
		CameraMode Interface
	*/
	virtual void UpdateView(float DeltaTime) override;

	/*
		member variables
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const UCurveVector> TargetOffSetCurve;
};
