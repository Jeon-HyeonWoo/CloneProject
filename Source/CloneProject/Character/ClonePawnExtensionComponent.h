// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "ClonePawnExtensionComponent.generated.h"


/**
 * 초기화 전반을 조정하는 Component
 * 따라서 ExtensionComponent는 관리를 받는 다른 Component보다 빠르게 생성되는 것을 보장받아야 한다.
 */
UCLASS()
class CLONEPROJECT_API UClonePawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
	
public:

	UClonePawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Feature Name 정의 */
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
};

/*
	PawnExtension, Character, Hero's Tick = false; : 주기적으로 검사x
	Event방식인 Delegate를 이용해서 Check.
*/
/*
	Unreal Engine은 Native C++ 먼저 생성되고, 그 다음 Blueprint이다.
	구지구지 Natvie C++로 모든걸 해결하고 싶으면 PostInitialize를 사용한다.
*/