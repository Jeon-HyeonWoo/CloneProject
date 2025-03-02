// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularCharacter.h"
#include "Components/GameFrameworkComponentManager.h"

void AModularCharacter::PreInitializeComponents()
{
	//BeingPlay ���� �ܰ�
	Super::PreInitializeComponents();
	//GameFeature���� Add Component���� ������ �� �ֵ��� ����.
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AModularCharacter::BeginPlay()
{
	//MudularCharacter�� ��ӹ޴� ��� �͵�(this), ���� Name_GameActorReady��� Evnet Name�ο�, ���
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void AModularCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//BeginPlay�� ��ϵ� ���� ����, ��ġ ������������
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}
