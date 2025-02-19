// Fill out your copyright notice in the Description page of Project Settings.


#include "ClonePawnExtensionComponent.h"

#include "CloneProject/CloneLogChannels.h"
#include "CloneProject/CloneGamePlayTags.h"
#include "Components/GameFrameworkComponentManager.h"

const FName UClonePawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UClonePawnExtensionComponent::UClonePawnExtensionComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UClonePawnExtensionComponent::OnRegister()
{
	//Component들을 부착하는 단계 매우매우 초기
	Super::OnRegister();

	//GetPawn = GetOwner
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogClone, Error, TEXT("this Component has been added to a BP Whose base class is not a Pawn!"));
		return;
	}

	//GameFrameworkComponentManager에 InitState 사용을 위해 등록 진행 
	// 등록은 상속 받았던 IGameFrameworkInitStateInterface Method RegisterInitStateFeature()를 활용
	/*
	*	FObjectKey : CDO에서 파생된 Instance의 고유번호를 구분할 수 있는 Key값 
	*
		내부적으로 OwningActor(Attached Actor, FeatureName, this(ExtComp))

		EX) A_Class의 Instance A1, A2가 있다고 가정, 각 Instance는 ExtComp, HeroComp를 지니고 있다.
		RegisterInitStateFeature를 사용하여 등록하면, 각 instance의 component를 구분지을 수 있다.
	*/
	//Feature Name이 없다면, 등록이 불가하다.
	RegisterInitStateFeature();

	// Debuging을 위한 함수
	//GetForActor = Actor의 World를 통해서 Manager가 Subsystem을 가져오는 것.
	UGameFrameworkComponentManager* Manager = UGameFrameworkComponentManager::GetForActor(GetOwningActor());
}

void UClonePawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	/*
		Actor의 InitState가 변경이 되면, 바뀐 상태를 받겠다.
		
		Params
		- Name_None을 넣으면, 모든 Component에 대해서 결과를 받겠다는 뜻. (관찰 대상)
		ex) if OwningActor has some components, 각각의 Component의 InitState변경사항을 받겠다.

		- FGameplayTag() : 기본값 을 넣으면, 특정 InitState만 받는게 아닌 모든 State단계에 걸리도록. (관찰 범위)

	*/
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);


	/*
		사용 이유 : InitState 변경 시도. Spawned로. 성공하면 넘어가고, 실패하면 못넘어간다.
		내부 코드 간략 : InitState를 변경이 가능한지 체크(CanChangeInitState)
		-> InitState가 변경하는 조건 함수 실행 (HandleChangeInitState)
		-> InitState변경 (ChangeFeatureInitState)
		-> Return BindOnActorInitStateChanged에 걸린 Delegate조건에 맞게 호출
		-> 호출되는 함수는 OnACtorinitStateChange
	*/
	ensure(TryToChangeInitState(FCloneGameplayTags::Get().InitState_Spawned));

	/*
		ForceUpdateInitState : 강제 업데이트
	*/
	CheckDefaultInitialization();

}

//instnace의 파괴, 소멸 시점.
void UClonePawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//파괴 전에 UnRegister를 해줘야한다.
	//Garbarge Collection에서 운이 좋게 제거가 된다면 좋겠지만, 그렇지 않으면 메모리 누수.
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UClonePawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	IGameFrameworkInitStateInterface::OnActorInitStateChanged(Params);
}

bool UClonePawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	const FCloneGameplayTags& InitTags = FCloneGameplayTags::Get();

	//CurrentState가 존재하지 않고, Bcuz : InitState_Spawned이 제일 초기부분, 바꾸려는 State가 Spawned인가?
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}

	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!PawnData)
		{
			return false;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		if (bIsLocallyControlled)
		{
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}

	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		/*
			HaveAll
			- Actor에 Bind된 모든 Feature(Component)들이 DataAvailable 상태일 때, DataInitialized로 넘어간다.
		*/
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}

	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}


	return false;
}

void UClonePawnExtensionComponent::CheckDefaultInitialization()
{
	/*
		implementers = this;
		MyActor = Owner;

		내부 코드 요약 : 
		
		- GetAllFeatureImplemeters(Implementers, MyActor, FGameplayTag(), MyFeatureName)
		를 통해 MyFeatureName(this Component)를 제외한 MyActor의 Component들을 가져온다.

		- 해당 Component들에 대해 CheckDefaultINitialization()을 호출.
		
	*/
	CheckDefaultInitializationForImplementers();
	

	const FCloneGameplayTags& InitTags = FCloneGameplayTags::Get();

	/*
	* 사용자 정의 InitState를 직접 넘겨주기 위한 TagArray
	*/
	static const TArray<FGameplayTag> StateChain = { 
		InitTags.InitState_Spawned, 
		InitTags.InitState_DataAvailable, 
		InitTags.InitState_DataInitialized,
		InitTags.InitState_GameplayReady
	};

	/*
		내부 코드 요약 :
		- 해당 Feature의 State가 CurrentState와 맞는지 체크
		- if(CanChangeInitState)를 통해 Update가 가능한 범위까지 계속 체크를 한다.
	*/
	ContinueInitStateChain(StateChain);

	/*
		CheckDefaultInitalizationForImplementers() = 나를 제외한 소유 Actor의 모든 Component의 상태
		ContinueInitStateChiain() = 나의 상태를 StateChain의 속성을 넣어 상태 체크
	*/
}
