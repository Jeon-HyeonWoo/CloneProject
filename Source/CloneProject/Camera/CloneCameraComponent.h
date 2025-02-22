// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CloneCameraComponent.generated.h"

class UCloneCameraModeStack;
class UCloneCameraMode;

/**
 *  ������ : World -> PlayerController -> (Update Camera ) -> PlayerCameraManager -> (CalculateCamera)
    -> Chracter -> (GetCameraView) -> CameraComponent -> HeroComponent
 * 
 *	CamaraComponent�� �Կ�(Collect Render Info), CameraManager�� ���(Render). 
 *	�� �̷��� �����? : CameraComponent�� Character�� ����������, 
 *	Controller�� ����� Character�� ������ CameraComponent���� Render�� �� �Ǿ������ �ȵǱ� ������,
 *  ���� ����� PlayerController(������ Player�� �����ϴ� �κ�)�� CameraManager�� ����ϰ� �ȴ�.
 */

/* template forward decalration */
template <class TClass> class TSubclassOf;

/* (Return Type, Delegate_Name) */
DECLARE_DELEGATE_RetVal(TSubclassOf<UCloneCameraMode>, FCloneCameraModeDelegate);


UCLASS()
class CLONEPROJECT_API UCloneCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
public:

	UCloneCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UCloneCameraComponent* FindCameraComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UCloneCameraComponent>() : nullptr);
	}

	/*
		member method
	*/
	AActor* GetTargetActor() const { return GetOwner(); } // = Character
	void UpdateCameraModes();

	/*
	* CameraComponent Interface
	*/
	//CameraComponent�� Character�� �����Ǵ� �ܰ�
	virtual void OnRegister() final;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

	/* Camera's Blending �� �����ϴ� Stack */
	UPROPERTY()
	TObjectPtr<UCloneCameraModeStack> CameraModeStack;

	/* Current CameraMode�� �������� Delegate */
	FCloneCameraModeDelegate DetermineCameraModeDelegate;
};
