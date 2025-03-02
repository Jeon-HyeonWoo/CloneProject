#pragma once

#include "Containers/Map.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeaturesAction_WorldActionBase.generated.h"

class FDelegateHandle;
class UGameInstance;
struct FGameFeatureActivatingContext;
struct FGameFeatureDeactivatingContext;
struct FWorldContext;

UCLASS(Abstract)
class UGameFeaturesAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()
public:

	/*
	* UGameFeatureAction's Interface
	*/
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;

	/*
	* Interface
	*/
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) PURE_VIRTUAL(GameFeaturesAction_WorldActionBase::AddToWorld, );
};
/*
	�ش� Ŭ������ ���� ����.
	- EngineSubsystem���� �߻��Ǵ� ������ � PI���� Ȯ���ϱ� ���� Ŭ����
*/