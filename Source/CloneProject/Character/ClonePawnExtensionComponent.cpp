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
	return IGameFrameworkInitStateInterface::CanChangeInitState(Manager, CurrentState, DesiredState);
}

void UClonePawnExtensionComponent::CheckDefaultInitialization()
{
	IGameFrameworkInitStateInterface::CheckDefaultInitialization();
}
