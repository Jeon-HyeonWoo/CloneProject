// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneCameraMode.h"
#include "ClonePlayerCameraManager.h"
#include "CloneCameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CloneCameraMode)

FCloneCameraModeView::FCloneCameraModeView()
	: Location(ForceInit)	//forceInit = 0
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(CLONE_CAMERA_DEFAULT_FOV)
{

}

void FCloneCameraModeView::Blend(const FCloneCameraModeView& Other, float OtherWeight)
{
	//최신 데이터는 보관할 필요없으니 Return
	if (OtherWeight <= 0.0f)
	{
		return;
	}
	else if(OtherWeight >= 1.0f)
	{
		//Weight가 더 작은 값이 있다면, 덮어쓴다.
		//Weight 가 1.0이면 Other를 덮어쓰면 된다.
		*this = Other;
		return;
	}

	//Location + OtherWeight * (Other.Location - Location);
	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);	
}

UCloneCameraMode::UCloneCameraMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FieldOfView = CLONE_CAMERA_DEFAULT_FOV;
	ViewPitchMin = CLONE_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = CLONE_CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;

	BlendFunction = ECloneCameraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
}

void UCloneCameraMode::UpdateCameraMode(float DeltaTime)
{
	//Actor를 활용하여, Pivot[Location][Rotation]을 계산하여, View를 Update
	UpdateView(DeltaTime);

	//BlendWeight를 DeltaTime을 이용하여, BlendAlpha 계산 후, BlendFunction에 맞게 Re-Maaping하여 최종 계산
	UpdateBlending(DeltaTime);
}

void UCloneCameraMode::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);
	
	View.Location = PivotLocation;
	View.Rotation = PivotRotation;

	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;
}

void UCloneCameraMode::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0.0f)
	{
		BlendAlpha += (DeltaTime / BlendTime);
	}
	else
	{
		BlendAlpha = 1.0f;
	}

	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;
	switch (BlendFunction)
	{
	case ECloneCameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;
	case ECloneCameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case ECloneCameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case ECloneCameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	default:
		checkf(false, TEXT("UpdateBlending : Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

UCloneCameraComponent* UCloneCameraMode::GetCloneCameraComponent() const
{
	//CameraMode and Stack Outer = CameraComponent
	return CastChecked<UCloneCameraComponent>(GetOuter());
}

AActor* UCloneCameraMode::GetTargetActor() const
{
	UCloneCameraComponent* CloneCameraComponent = GetCloneCameraComponent();
	return CloneCameraComponent->GetTargetActor();
}

FVector UCloneCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetPawnViewLocation();
	}

	return FVector();
}

FRotator UCloneCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetViewRotation(); //Controller's Rotation
	}

	return TargetActor->GetActorRotation();
}

UCloneCameraModeStack::UCloneCameraModeStack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UCloneCameraMode* UCloneCameraModeStack::GetCameraModeInstance(TSubclassOf<UCloneCameraMode>& CameraModeClass)
{
	//1. chec null
	check(CameraModeClass);

	//2. Circuit CameraModeInstance
	for (UCloneCameraMode* CameraMode : CameraModeInstance)
	{
		//3. Instance has same CameraMode? = return that Instance Mode
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	//4. Instance has not Param, NewObject.
	UCloneCameraMode* NewCameraMode = NewObject<UCloneCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);

	//5. null check
	check(NewCameraMode);

	//6. Add NewCameraMode
	CameraModeInstance.Add(NewCameraMode);

	return NewCameraMode;
}

void UCloneCameraModeStack::PushCameraMode(TSubclassOf<UCloneCameraMode> CameraModeClass)
{
	if (!CameraModeClass)
	{
		return;
	}

	UCloneCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 StackSize = CameraModeStack.Num();
	//	CameraModeStack[0] = Recent CameraMode -> Return;
	//
	//GetCameraModeInstnace의 CameraMode는 가장 최신값, Stack은 가장 최신값이 위에있어야 하는데 아래에 있는 이유는?
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		return;
	}

	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.0f;

	/*
	*	CDF : 누적분포함수, 주어진 확률변수가 특정 값보다 작거나 같은 확률.
	*	0 : 최신 값, 1 : 나중값
	*	시간 경과에 따라 확정적으로 Mode의 Weight를 비율로 나누어 보간
	* 
		BlendWeight		|		ExistingStackContirubution		|		ExistingStackContruibution (accumulated)
		0.1f			|		(1.0f) * 0.1f = 1.0f			|		(1.0f - 0.1f) = 0.9f
		0.3f			|		(0.9f) * 0.3f = 0.27f			|		(1.0f - 0.3f) * 0.9f = 0.63f
		0.6f			|		(0.63f) * 0.6f = 0.378f			|		(1.0f - 0.6f) * 0.63f = 0.252f
		1.0f			|		(0.252f) * 1.0f = 0.252f		|		
						|		0.1f + 0.27f + 0.378f + 0.252f = 1.0f
	*/

	/* 
		CameraModeStack의 Weight값 보간
	*/
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			ExistingStackIndex = StackIndex;
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
		}
	}

	//Index값이 None이 아니라면, = 인덱스 값을 찾았다면,
	if (ExistingStackIndex != INDEX_NONE)
	{
		//같은 CameraMode를 찾았다는 것임으로 삭제, 삭제된 만큼 Size감소,
		CameraModeStack.RemoveAt(ExistingStackIndex); 
		StackSize--;
	}
	else //찾지 않았다면,
	{
		//새로들어온 Data임으로 비율을 계산할 필요가 없음으로 0.0;
		ExistingStackContribution = 0.0f;
	}

	//Blend를 해야할지에 대한 bool값, blendtime값이 존재하고, Stack에 다른 CameraMode가 있다면,
	const bool bShouldBlend = ((CameraMode->BlendTime > 0.0f) && (StackSize > 0));
	// 보간을 해야한다면? 0.0, 보간을 안해도 된다면? 1.0
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->BlendWeight = BlendWeight;

	//제일 최근에 주입되는 CameraMode의 Index를 제일 처음으로(0번)으로 넣어줌
	CameraModeStack.Insert(CameraMode, 0); 

	//가장 오래된 Stack의 BlendWeight의 값은 1.0f 
	CameraModeStack.Last()->BlendWeight = 1.0f;
}

void UCloneCameraModeStack::EvaluateStack(float DeltaTime, FCloneCameraModeView& OutCameraModeView)
{
	//Top->Bottom [ 0 -> Num ]까지 순차적으로 Stack에 있는 CameraMode업데이트
	//최신 데이터 -> 과거 데이터 
	UpdateStack(DeltaTime);

	//Bottom->Top 가장 느린데이터 -> 빠른데이터
	BlendStack(OutCameraModeView);
}

void UCloneCameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	//if CameraStack is Empty -> return
	if (StackSize <= 0)
	{
		return;
	}

	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;

	//loop for Stack (0 = recent data : Current CameraMode) -> (1 = old data)
	//Weight value > 1.0 -> use not enough caching -> RemoveAt CameraMode in stack
	//Reason why stack is reversed, -> compotable to Remove CameraMode in stack
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UCloneCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		CameraMode->UpdateCameraMode(DeltaTime);

		if (CameraMode->BlendWeight >= 1.0f)
		{
			//to keep the most recently data
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UCloneCameraModeStack::BlendStack(FCloneCameraModeView& OutCameraModeView) const
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	const UCloneCameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	//Stack.Last View
	OutCameraModeView = CameraMode->View; 
	
	//Loop, Stack.Last -> Stack.Begin
	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);
		//과거 데이터부터 현재 데이터 까지 Blend
		OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);
	}
}


