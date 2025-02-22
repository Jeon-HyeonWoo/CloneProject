// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CloneCameraMode.generated.h"

class UCloneCameraComponent;
/*
	FCloneCameraView : Rendering Area 
	-> Camera�� ���������� ���� ���� ������ �����ϴ� ����ü
*/

struct FCloneCameraModeView
{
	FCloneCameraModeView();

	void Blend(const FCloneCameraModeView& Other, float OtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/*
	[0, 1]�� BlendFunction�� �°� ����� ��������
*/
UENUM(BlueprintType)
enum class ECloneCameraModeBlendFunction : uint8
{
	Linear,		//����
	/*
		EaseIn/Out�� Exponent ���� ���� ����
	*/
	EaseIn,		//õõ�� ���� -> ������ ����
	EaseOut,	//������ ���� -> õõ�� ����
	EaseInOut,	// y = x^3 ����
	COUNT
};

/*
*	Lyra�� �ñ��� ��ǥ : ���ȭ.
*	ex) Mode_FPS, Mode_InCar, Mode_RPG..

	CameraModeStack : CameraMode�� Stackȭ
	Stackȭ�� ���� : Camera�� �ڿ������� ��ȯ�� ���� Mode���� �� ����
*/
UCLASS(Abstract, NotBlueprintable)
class CLONEPROJECT_API UCloneCameraMode : public UObject
{
	GENERATED_BODY()
	
public:

	UCloneCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* 
		member method
	*/
	void UpdateCameraMode(float DeltaTime);
	virtual void UpdateView(float DeltaTime);
	void UpdateBlending(float DeltaTime);

	UCloneCameraComponent* GetCloneCameraComponent() const;
	AActor* GetTargetActor() const;
	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;

	/* 
		member Variable
	*/
	FCloneCameraModeView View;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.0", ClampMax = "89.9"))
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;




	/* blend time */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	/* ���� Blend�� [0, 1] */
	float BlendAlpha;

	/* �ش� CameraMode�� ���� blend��
		�ռ� BlendAlpha�� ���� ���� �����Ͽ� ���� BlendWeight�� ��� 
	*/
	float BlendWeight;

	/* 
		EaseIn/Out�� ����� Exponent(����)
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	ECloneCameraModeBlendFunction BlendFunction;
};

/* for Camera Blending Obj*/
UCLASS()
class UCloneCameraModeStack : public UObject
{
	GENERATED_BODY()

public:

	UCloneCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
		membermethod
	*/
	UCloneCameraMode* GetCameraModeInstance(TSubclassOf<UCloneCameraMode>& CameraModeClass);
	void PushCameraMode(TSubclassOf<UCloneCameraMode> CameraModeClass);
	void EvaluateStack(float DeltaTime, FCloneCameraModeView& OutCameraModeView);
	void UpdateStack(float DeltaTime);
	void BlendStack(FCloneCameraModeView& OutCameraModeView) const;

	/*
	 * member variables 
	 */
	//������ CameraMode ����, CameraMode Caching
	UPROPERTY() 
	TArray<TObjectPtr<UCloneCameraMode>> CameraModeInstance;

	//Camera Matrix Blend ������Ʈ
	UPROPERTY()
	TArray<TObjectPtr<UCloneCameraMode>> CameraModeStack;

	
};

