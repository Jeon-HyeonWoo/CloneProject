// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "CloneHeroComponent.generated.h"

/**
 * Component that sets up input and camera handling for player controlled pawns (or bots that simulate players)
 * - this depends on a pawnextensioncomponent to coordinate initalization
 * 
 * Camrea Input.. Can player Controlled system initalization
 */
/*
	보통의 경우 Character나 PlayerController에서 SetupInputComponent를 이용해서 Input을 제어 했다면,
	이제는 한 Character에서 탈부착이 가능한(Component) 것들을 Setting한다 
*/
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class CLONEPROJECT_API UCloneHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
	
public:

	UCloneHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* FeatureName 정의 .cpp에서 */
	static const FName Name_ActorFeatureName;


	/* UPawnCompoent Interface */
	virtual void OnReigster() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	/* IGameFrameworkInitStateInterface */
	virtual FName GetFeatureName() const final { return Name_ActorFeatureName; }
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) final;
	virtual void CheckDefaultInitialization() final;

	
};
