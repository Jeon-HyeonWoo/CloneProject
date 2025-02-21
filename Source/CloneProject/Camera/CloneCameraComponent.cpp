// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneCameraComponent.h"

#include "CloneCameraMode.h"

UCloneCameraComponent::UCloneCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), CameraModeStack(nullptr)
{

}

void UCloneCameraComponent::OnRegister()
{
	Super::OnRegister();

	if (!CameraModeStack)
	{
		CameraModeStack = NewObject<UCloneCameraModeStack>(this);
	}
}

void UCloneCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	check(CameraModeStack);

	UpdateCameraModes();
}

void UCloneCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	//DetermineCameraModeDelegate가 bind가 잘 되어있다면,
	if (DetermineCameraModeDelegate.IsBound())
	{
		//Delegate가 되어있는 함수가 실행이 되면서 CameraMode를 반환한다.
		if (TSubclassOf<UCloneCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
		{
			//그 CameraMode를 매 프레임 저장.
			//CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}