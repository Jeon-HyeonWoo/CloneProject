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
	������ ��� Character�� PlayerController���� SetupInputComponent�� �̿��ؼ� Input�� ���� �ߴٸ�,
	������ �� Character���� Ż������ ������(Component) �͵��� Setting�Ѵ� 
*/
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class CLONEPROJECT_API UCloneHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
	
public:

	UCloneHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* FeatureName ���� .cpp���� */
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
