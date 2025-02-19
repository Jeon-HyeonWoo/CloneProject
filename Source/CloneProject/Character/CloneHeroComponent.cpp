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

		if (!GetPawn<APawn>()) // OwnerPawn이 존재하지 않는다면,
		{
			UE_LOG(LogClone, Error, TEXT("This Component has been added to a BP Whose base class is not a pawn"));
			return;
		}

	}

	//InitState를 부착! FeatureName은 필수!
	RegisterInitStateFeature();
}

void UCloneHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	//ExtensionComponent의 State만 확인, bcuz : ExtComponent는 Manager의 역할을 하기 때문에 다른 Component들은 ExtComp에서 확인가능.
	BindOnActorInitStateChanged(UClonePawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	//InitState_Spawned로 초기화
	ensure(TryToChangeInitState(FCloneGameplayTags::Get().InitState_Spawned));

	//ForceUpdate
	CheckDefaultInitialization();
}

void UCloneHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//동적할당 처럼 Register를 했다면, UnRegister해줘야함!
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
		ExtComp와의 차이점 : CheckDefaultInitializationForImplementers()의 부재.
		이유 : 나를 제외한 모든 Component들을 CheckDafaultInitailization()호출의 의미인 ForImplementers는
		Manager인 ExtComp에게만 필요하기 때문.
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

