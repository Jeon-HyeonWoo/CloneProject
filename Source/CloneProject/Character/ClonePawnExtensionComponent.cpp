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
	//Component���� �����ϴ� �ܰ� �ſ�ſ� �ʱ�
	Super::OnRegister();

	//GetPawn = GetOwner
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogClone, Error, TEXT("this Component has been added to a BP Whose base class is not a Pawn!"));
		return;
	}

	//GameFrameworkComponentManager�� InitState ����� ���� ��� ���� 
	// ����� ��� �޾Ҵ� IGameFrameworkInitStateInterface Method RegisterInitStateFeature()�� Ȱ��
	/*
	*	FObjectKey : CDO���� �Ļ��� Instance�� ������ȣ�� ������ �� �ִ� Key�� 
	*
		���������� OwningActor(Attached Actor, FeatureName, this(ExtComp))

		EX) A_Class�� Instance A1, A2�� �ִٰ� ����, �� Instance�� ExtComp, HeroComp�� ���ϰ� �ִ�.
		RegisterInitStateFeature�� ����Ͽ� ����ϸ�, �� instance�� component�� �������� �� �ִ�.
	*/
	//Feature Name�� ���ٸ�, ����� �Ұ��ϴ�.
	RegisterInitStateFeature();

	// Debuging�� ���� �Լ�
	//GetForActor = Actor�� World�� ���ؼ� Manager�� Subsystem�� �������� ��.
	UGameFrameworkComponentManager* Manager = UGameFrameworkComponentManager::GetForActor(GetOwningActor());
}

void UClonePawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	/*
		Actor�� InitState�� ������ �Ǹ�, �ٲ� ���¸� �ްڴ�.
		
		Params
		- Name_None�� ������, ��� Component�� ���ؼ� ����� �ްڴٴ� ��. (���� ���)
		ex) if OwningActor has some components, ������ Component�� InitState��������� �ްڴ�.

		- FGameplayTag() : �⺻�� �� ������, Ư�� InitState�� �޴°� �ƴ� ��� State�ܰ迡 �ɸ�����. (���� ����)

	*/
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);


	/*
		��� ���� : InitState ���� �õ�. Spawned��. �����ϸ� �Ѿ��, �����ϸ� ���Ѿ��.
		���� �ڵ� ���� : InitState�� ������ �������� üũ(CanChangeInitState)
		-> InitState�� �����ϴ� ���� �Լ� ���� (HandleChangeInitState)
		-> InitState���� (ChangeFeatureInitState)
		-> Return BindOnActorInitStateChanged�� �ɸ� Delegate���ǿ� �°� ȣ��
		-> ȣ��Ǵ� �Լ��� OnACtorinitStateChange
	*/
	ensure(TryToChangeInitState(FCloneGameplayTags::Get().InitState_Spawned));

	/*
		ForceUpdateInitState : ���� ������Ʈ
	*/
	CheckDefaultInitialization();

}

//instnace�� �ı�, �Ҹ� ����.
void UClonePawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//�ı� ���� UnRegister�� ������Ѵ�.
	//Garbarge Collection���� ���� ���� ���Ű� �ȴٸ� ��������, �׷��� ������ �޸� ����.
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

	//CurrentState�� �������� �ʰ�, Bcuz : InitState_Spawned�� ���� �ʱ�κ�, �ٲٷ��� State�� Spawned�ΰ�?
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
			- Actor�� Bind�� ��� Feature(Component)���� DataAvailable ������ ��, DataInitialized�� �Ѿ��.
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

		���� �ڵ� ��� : 
		
		- GetAllFeatureImplemeters(Implementers, MyActor, FGameplayTag(), MyFeatureName)
		�� ���� MyFeatureName(this Component)�� ������ MyActor�� Component���� �����´�.

		- �ش� Component�鿡 ���� CheckDefaultINitialization()�� ȣ��.
		
	*/
	CheckDefaultInitializationForImplementers();
	

	const FCloneGameplayTags& InitTags = FCloneGameplayTags::Get();

	/*
	* ����� ���� InitState�� ���� �Ѱ��ֱ� ���� TagArray
	*/
	static const TArray<FGameplayTag> StateChain = { 
		InitTags.InitState_Spawned, 
		InitTags.InitState_DataAvailable, 
		InitTags.InitState_DataInitialized,
		InitTags.InitState_GameplayReady
	};

	/*
		���� �ڵ� ��� :
		- �ش� Feature�� State�� CurrentState�� �´��� üũ
		- if(CanChangeInitState)�� ���� Update�� ������ �������� ��� üũ�� �Ѵ�.
	*/
	ContinueInitStateChain(StateChain);

	/*
		CheckDefaultInitalizationForImplementers() = ���� ������ ���� Actor�� ��� Component�� ����
		ContinueInitStateChiain() = ���� ���¸� StateChain�� �Ӽ��� �־� ���� üũ
	*/
}
