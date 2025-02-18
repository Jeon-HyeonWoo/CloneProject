// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneGameInstance.h"
#include "Components/GameFrameworkComponentManager.h" //Managing Components / Plugin Modular GameplayActor
#include "CloneProject/CloneGamePlayTags.h"

void UCloneGameInstance::Init()
{
	Super::Init();

	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
	if (ensure(ComponentManager))
	{
		const FCloneGameplayTags& GameplayTags = FCloneGameplayTags::Get();
		
		//부품(Components)의 순서를 보장해주지 않기 때문에 ReigsterInitState에서 초기화 순서를 정해주어야한다.
		ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_DataAvailable);
		ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
	}
}

void UCloneGameInstance::Shutdown()
{
	Super::Shutdown();
}
