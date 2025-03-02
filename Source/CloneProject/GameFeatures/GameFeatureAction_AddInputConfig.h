#pragma once

#include "CoreMinimal.h"
#include "GameFeaturesAction_WorldActionBase.h"
#include "CloneProject/Input/CloneMappableConfigPair.h"
#include "GameFeatureAction_AddInputConfig.generated.h"

/* forward declarations */
struct FComponentRequestHandle;
class APawn;

UCLASS()
class UGameFeatureAction_AddInputConfig : public UGameFeaturesAction_WorldActionBase
{
	GENERATED_BODY()

public:

	/*
	* UGameFeatureAction Interface
	*/
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	/*
	* UGameFeatureAction_WorldActionBase Interface
	*/
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

private:

	/* Pawn을 캐싱해 두면 World전체의 Pawn을 찾지 않아도 되기 때문에 만든 Struct */
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};

	/* GameFeatureState가 변경 될 때, CallBack함수. (ExtensionEvent를 GameFrameworkComponentManager를 통해 호출 가능) */
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	/* InputConfig를 EnhancedInputLocalPlayerSubsystem에 추가 및 제거 */
	void AddInputConfig(APawn* Pawn, FPerContextData& ActiveData);
	void RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData);

	/* ActiveData 초기화 */
	void Reset(FPerContextData& ActiveData);

	/* world -> (extension, request, pawn) */
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	/* PlayerMappableInputConfig를 들고 있는 맴버 변수 
	* - config : has MappingContext
	*/
	UPROPERTY(EditAnywhere)
	TArray<FCloneMappableConfigPair> InputConfigs;
};