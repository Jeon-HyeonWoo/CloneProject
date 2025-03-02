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

	/* Pawn�� ĳ���� �θ� World��ü�� Pawn�� ã�� �ʾƵ� �Ǳ� ������ ���� Struct */
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};

	/* GameFeatureState�� ���� �� ��, CallBack�Լ�. (ExtensionEvent�� GameFrameworkComponentManager�� ���� ȣ�� ����) */
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	/* InputConfig�� EnhancedInputLocalPlayerSubsystem�� �߰� �� ���� */
	void AddInputConfig(APawn* Pawn, FPerContextData& ActiveData);
	void RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData);

	/* ActiveData �ʱ�ȭ */
	void Reset(FPerContextData& ActiveData);

	/* world -> (extension, request, pawn) */
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	/* PlayerMappableInputConfig�� ��� �ִ� �ɹ� ���� 
	* - config : has MappingContext
	*/
	UPROPERTY(EditAnywhere)
	TArray<FCloneMappableConfigPair> InputConfigs;
};