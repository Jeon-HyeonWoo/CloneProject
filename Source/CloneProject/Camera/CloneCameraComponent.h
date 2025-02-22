// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CloneCameraComponent.generated.h"

class UCloneCameraModeStack;
class UCloneCameraMode;

/**
 *  순서도 : World -> PlayerController -> (Update Camera ) -> PlayerCameraManager -> (CalculateCamera)
    -> Chracter -> (GetCameraView) -> CameraComponent -> HeroComponent
 * 
 *	CamaraComponent는 촬영(Collect Render Info), CameraManager는 출력(Render). 
 *	왜 이러한 방법을? : CameraComponent는 Character에 부착되지만, 
 *	Controller의 변경시 Character에 부착된 CameraComponent까지 Render가 다 되어버리면 안되기 때문에,
 *  최종 출력은 PlayerController(실제로 Player가 조작하는 부분)의 CameraManager가 담당하게 된다.
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
	//CameraComponent가 Character에 부착되는 단계
	virtual void OnRegister() final;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

	/* Camera's Blending 을 지원하는 Stack */
	UPROPERTY()
	TObjectPtr<UCloneCameraModeStack> CameraModeStack;

	/* Current CameraMode를 가져오는 Delegate */
	FCloneCameraModeDelegate DetermineCameraModeDelegate;
};
