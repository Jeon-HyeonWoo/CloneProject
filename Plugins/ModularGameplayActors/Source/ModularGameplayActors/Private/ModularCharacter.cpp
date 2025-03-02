// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularCharacter.h"
#include "Components/GameFrameworkComponentManager.h"

void AModularCharacter::PreInitializeComponents()
{
	//BeingPlay 이전 단계
	Super::PreInitializeComponents();
	//GameFeature에서 Add Component등을 수신할 수 있도록 설정.
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AModularCharacter::BeginPlay()
{
	//MudularCharacter를 상속받는 모든 것들(this), 에게 Name_GameActorReady라는 Evnet Name부여, 등록
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void AModularCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//BeginPlay에 등록된 것을 해제, 마치 동적생성마냥
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}
