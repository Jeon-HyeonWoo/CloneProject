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
		
		//��ǰ(Components)�� ������ ���������� �ʱ� ������ ReigsterInitState���� �ʱ�ȭ ������ �����־���Ѵ�.
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
