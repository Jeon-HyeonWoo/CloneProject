// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "ClonePawnExtensionComponent.generated.h"

class UClonePawnData;

/**
 * �ʱ�ȭ ������ �����ϴ� Component
 * ���� ExtensionComponent�� ������ �޴� �ٸ� Component���� ������ �����Ǵ� ���� ����޾ƾ� �Ѵ�.
 */
UCLASS()
class CLONEPROJECT_API UClonePawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
	
public:

	UClonePawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Feature Name ���� */
	static const FName NAME_ActorFeatureName;

	/*
		UPawnComponent Interface
	*/
	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;
	
	/*
		IGameFrameworkInitStateInterface 
	*/
	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void CheckDefaultInitialization() final;

	/* for PawnData Casing */
	UPROPERTY(EditInstanceOnly, Category = "Clone|Pawn")
	TObjectPtr<const UClonePawnData> PawnData;
};

/*
	PawnExtension, Character, Hero's Tick = false; : �ֱ������� �˻�x
	Event����� Delegate�� �̿��ؼ� Check.
*/
/*
	Unreal Engine�� Native C++ ���� �����ǰ�, �� ���� Blueprint�̴�.
	�������� Natvie C++�� ���� �ذ��ϰ� ������ PostInitialize�� ����Ѵ�.
*/

/*
		���� :
		- PawnExtesionComponent�� BindOnActorInitStateChanged(NameNone, Tag()) �� ���� ��� ���¸� ����
		- ������ Tags�� OnActorInitStateChanged�� ���� ���� ������ �Ǿ����� �� �� �ִ�.
		- TryToChangeInitState(MyGameplayTags::Get().MyState.Value)�� ���� ��� ����(MyState.Value)�� ���� ������ �õ�.
		- TryToChagneInitState���� �õ��ϸ�, ���������� CanChangeInitState�� ���� ���� �� �ִ��� üũ.
		- CanChangeInitState�� Ture���, HandleChagneInitState�� ���� �Ѿ�� ������ �����ؾ��Ѵ�.
		- Handle�� ������ �Ѱ�ٸ�, BindOnActorInitStateChanged���� Delegate�� ȣ��.
		- ���������� CheckDefaultInitialization�� ���� ������ ������Ʈ�� �����Ѵ�.
		- ���� ������Ʈ�� ������ �Ʒ��� ����.
		- CheckDeafultInitalization�� Implenters�ڵ带 ������� ���� ������ ��� Component���� Check�ϰ�,
		- ContinueInitStateChain�� ���� �� �ڽ��� Update�� ������ �κб��� ��� Check

		* PawnExtensionComponent�� OwnerActor�� Component�� �ʱ�ȭ ����ϱ� ������ Handle�� ����.
	*/