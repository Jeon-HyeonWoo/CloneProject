// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CloneCameraMode.generated.h"

class UCloneCameraComponent;
/*
	FCloneCameraView : Rendering Area 
	-> Camera로 최종적으로 보고 싶은 영역을 저장하는 구조체
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
	[0, 1]을 BlendFunction에 맞게 재매핑 선형보간
*/
UENUM(BlueprintType)
enum class ECloneCameraModeBlendFunction : uint8
{
	Linear,		//선형
	/*
		EaseIn/Out은 Exponent 값에 의해 조절
	*/
	EaseIn,		//천천히 증가 -> 빠르게 증가
	EaseOut,	//빠르게 증가 -> 천천히 증가
	EaseInOut,	// y = x^3 형태
	COUNT
};

/*
*	Lyra의 궁극적 목표 : 모듈화.
*	ex) Mode_FPS, Mode_InCar, Mode_RPG..

	CameraModeStack : CameraMode를 Stack화
	Stack화의 이유 : Camera의 자연스러운 전환을 위해 Mode변경 시 보간
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

	/* 선형 Blend값 [0, 1] */
	float BlendAlpha;

	/* 해당 CameraMode의 최종 blend값
		앞선 BlendAlpha의 선형 값을 매핑하여 최종 BlendWeight를 계산 
	*/
	float BlendWeight;

	/* 
		EaseIn/Out에 사용한 Exponent(지수)
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
	//생성된 CameraMode 관리, CameraMode Caching
	UPROPERTY() 
	TArray<TObjectPtr<UCloneCameraMode>> CameraModeInstance;

	//Camera Matrix Blend 업데이트
	UPROPERTY()
	TArray<TObjectPtr<UCloneCameraMode>> CameraModeStack;

	
};

