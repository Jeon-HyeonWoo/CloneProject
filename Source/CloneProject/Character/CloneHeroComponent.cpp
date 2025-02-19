// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneHeroComponent.h"

#include "CloneProject/CloneLogChannels.h"
#include "CloneProject/CloneGamePlayTags.h"
#include "CloneProject/Character/ClonePawnExtensionComponent.h"
#include "CloneProject/Player/ClonePlayerState.h"
#include "Components/GameFrameworkComponentManager.h"


const FName UCloneHeroComponent::Name_ActorFeatureName("HeroComponent");

UCloneHeroComponent::UCloneHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UCloneHeroComponent::OnReigster()
{
	Super::OnRegister();

	{

		if (!GetPawn<APawn>()) // OwnerPawn�� �������� �ʴ´ٸ�,
		{
			UE_LOG(LogClone, Error, TEXT("This Component has been added to a BP Whose base class is not a pawn"));
			return;
		}

	}

	//InitState�� ����! FeatureName�� �ʼ�!
	RegisterInitStateFeature();
}

void UCloneHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	//ExtensionComponent�� State�� Ȯ��, bcuz : ExtComponent�� Manager�� ������ �ϱ� ������ �ٸ� Component���� ExtComp���� Ȯ�ΰ���.
	BindOnActorInitStateChanged(UClonePawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	//InitState_Spawned�� �ʱ�ȭ
	ensure(TryToChangeInitState(FCloneGameplayTags::Get().InitState_Spawned));

	//ForceUpdate
	CheckDefaultInitialization();
}

void UCloneHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//�����Ҵ� ó�� Register�� �ߴٸ�, UnRegister�������!
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}


void UCloneHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	IGameFrameworkInitStateInterface::OnActorInitStateChanged(Params);
}

bool UCloneHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FCloneGameplayTags& InitTags = FCloneGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	AClonePlayerState* ClonePS = GetPlayerState<AClonePlayerState>();

	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
		{

		}
	}


	return IGameFrameworkInitStateInterface::CanChangeInitState(Manager, CurrentState, DesiredState);
}

void UCloneHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	IGameFrameworkInitStateInterface::HandleChangeInitState(Manager, CurrentState, DesiredState);
}

void UCloneHeroComponent::CheckDefaultInitialization()
{
	/*
		ExtComp���� ������ : CheckDefaultInitializationForImplementers()�� ����.
		���� : ���� ������ ��� Component���� CheckDafaultInitailization()ȣ���� �ǹ��� ForImplementers��
		Manager�� ExtComp���Ը� �ʿ��ϱ� ����.
	*/

	const FCloneGameplayTags& InitTags = FCloneGameplayTags::Get();

	static const TArray<FGameplayTag> StateChain = {
		InitTags.InitState_Spawned,
		InitTags.InitState_DataAvailable,
		InitTags.InitState_DataInitialized,
		InitTags.InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}

