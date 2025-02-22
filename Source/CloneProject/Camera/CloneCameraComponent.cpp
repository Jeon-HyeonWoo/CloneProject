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

	//Blending�� ���� ����
	FCloneCameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
		{
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);

	FieldOfView = CameraModeView.FieldOfView;

	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}
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
			CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}