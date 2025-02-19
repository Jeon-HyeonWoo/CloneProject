// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "ClonePawnExtensionComponent.generated.h"

class UClonePawnData;

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

	/* for PawnData Casing */
	UPROPERTY(EditInstanceOnly, Category = "Clone|Pawn")
	TObjectPtr<const UClonePawnData> PawnData;
};

/*
	PawnExtension, Character, Hero's Tick = false; : 주기적으로 검사x
	Event방식인 Delegate를 이용해서 Check.
*/
/*
	Unreal Engine은 Native C++ 먼저 생성되고, 그 다음 Blueprint이다.
	구지구지 Natvie C++로 모든걸 해결하고 싶으면 PostInitialize를 사용한다.
*/

/*
		정리 :
		- PawnExtesionComponent는 BindOnActorInitStateChanged(NameNone, Tag()) 를 통해 모든 상태를 관찰
		- 관찰된 Tags는 OnActorInitStateChanged를 통해 누가 변경이 되었는지 알 수 있다.
		- TryToChangeInitState(MyGameplayTags::Get().MyState.Value)를 통해 어느 상태(MyState.Value)로 변할 것인지 시도.
		- TryToChagneInitState에서 시도하면, 내부적으로 CanChangeInitState를 통해 변할 수 있는지 체크.
		- CanChangeInitState가 Ture라면, HandleChagneInitState를 통해 넘어가는 조건을 설정해야한다.
		- Handle의 조건을 넘겼다면, BindOnActorInitStateChanged에서 Delegate를 호출.
		- 마지막으로 CheckDefaultInitialization을 통해 강제로 업데이트를 진행한다.
		- 강제 업데이트인 이유는 아래와 같다.
		- CheckDeafultInitalization은 Implenters코드를 실행시켜 나를 제외한 모든 Component들을 Check하고,
		- ContinueInitStateChain을 통해 내 자신이 Update가 가능한 부분까지 계속 Check

		* PawnExtensionComponent는 OwnerActor의 Component를 초기화 담당하기 때문에 Handle이 없다.
	*/