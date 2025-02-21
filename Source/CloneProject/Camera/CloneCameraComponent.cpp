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

	//DetermineCameraModeDelegate�� bind�� �� �Ǿ��ִٸ�,
	if (DetermineCameraModeDelegate.IsBound())
	{
		//Delegate�� �Ǿ��ִ� �Լ��� ������ �Ǹ鼭 CameraMode�� ��ȯ�Ѵ�.
		if (TSubclassOf<UCloneCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
		{
			//�� CameraMode�� �� ������ ����.
			//CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}