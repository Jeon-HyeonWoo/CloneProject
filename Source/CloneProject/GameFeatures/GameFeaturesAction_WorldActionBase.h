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
	해당 클래스의 존재 의의.
	- EngineSubsystem에서 발생되는 문제인 어떤 PI인지 확인하기 위한 클래스
*/