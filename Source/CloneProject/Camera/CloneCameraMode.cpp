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
	//�ֽ� �����ʹ� ������ �ʿ������ Return
	if (OtherWeight <= 0.0f)
	{
		return;
	}
	else if(OtherWeight >= 1.0f)
	{
		//Weight�� �� ���� ���� �ִٸ�, �����.
		//Weight �� 1.0�̸� Other�� ����� �ȴ�.
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
	//Actor�� Ȱ���Ͽ�, Pivot[Location][Rotation]�� ����Ͽ�, View�� Update
	UpdateView(DeltaTime);

	//BlendWeight�� DeltaTime�� �̿��Ͽ�, BlendAlpha ��� ��, BlendFunction�� �°� Re-Maaping�Ͽ� ���� ���
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
	//GetCameraModeInstnace�� CameraMode�� ���� �ֽŰ�, Stack�� ���� �ֽŰ��� �����־�� �ϴµ� �Ʒ��� �ִ� ������?
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		return;
	}

	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.0f;

	/*
	*	CDF : ���������Լ�, �־��� Ȯ�������� Ư�� ������ �۰ų� ���� Ȯ��.
	*	0 : �ֽ� ��, 1 : ���߰�
	*	�ð� ����� ���� Ȯ�������� Mode�� Weight�� ������ ������ ����
	* 
		BlendWeight		|		ExistingStackContirubution		|		ExistingStackContruibution (accumulated)
		0.1f			|		(1.0f) * 0.1f = 1.0f			|		(1.0f - 0.1f) = 0.9f
		0.3f			|		(0.9f) * 0.3f = 0.27f			|		(1.0f - 0.3f) * 0.9f = 0.63f
		0.6f			|		(0.63f) * 0.6f = 0.378f			|		(1.0f - 0.6f) * 0.63f = 0.252f
		1.0f			|		(0.252f) * 1.0f = 0.252f		|		
						|		0.1f + 0.27f + 0.378f + 0.252f = 1.0f
	*/

	/* 
		CameraModeStack�� Weight�� ����
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

	//Index���� None�� �ƴ϶��, = �ε��� ���� ã�Ҵٸ�,
	if (ExistingStackIndex != INDEX_NONE)
	{
		//���� CameraMode�� ã�Ҵٴ� �������� ����, ������ ��ŭ Size����,
		CameraModeStack.RemoveAt(ExistingStackIndex); 
		StackSize--;
	}
	else //ã�� �ʾҴٸ�,
	{
		//���ε��� Data������ ������ ����� �ʿ䰡 �������� 0.0;
		ExistingStackContribution = 0.0f;
	}

	//Blend�� �ؾ������� ���� bool��, blendtime���� �����ϰ�, Stack�� �ٸ� CameraMode�� �ִٸ�,
	const bool bShouldBlend = ((CameraMode->BlendTime > 0.0f) && (StackSize > 0));
	// ������ �ؾ��Ѵٸ�? 0.0, ������ ���ص� �ȴٸ�? 1.0
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->BlendWeight = BlendWeight;

	//���� �ֱٿ� ���ԵǴ� CameraMode�� Index�� ���� ó������(0��)���� �־���
	CameraModeStack.Insert(CameraMode, 0); 

	//���� ������ Stack�� BlendWeight�� ���� 1.0f 
	CameraModeStack.Last()->BlendWeight = 1.0f;
}

void UCloneCameraModeStack::EvaluateStack(float DeltaTime, FCloneCameraModeView& OutCameraModeView)
{
	//Top->Bottom [ 0 -> Num ]���� ���������� Stack�� �ִ� CameraMode������Ʈ
	//�ֽ� ������ -> ���� ������ 
	UpdateStack(DeltaTime);

	//Bottom->Top ���� ���������� -> ����������
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
		//���� �����ͺ��� ���� ������ ���� Blend
		OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);
	}
}


