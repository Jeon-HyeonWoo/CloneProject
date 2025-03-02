#include "GameFeatureAction_AddInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
#include "Components/GameFrameworkComponentManager.h"
#include "CloneProject/Character/CloneHeroComponent.h"

void UGameFeatureAction_AddInputConfig::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	//혹시 초기화가 되지 않았나 한 번 더 체크 한 이후 WorldActionBase의 Activating 실행
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
		!ensure(ActiveData.PawnsAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputConfig::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
	FPerContextData* ActiveData = ContextData.Find(Context);
	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

void UGameFeatureAction_AddInputConfig::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (GameInstance && World && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			//추가적인 요청에 대한 Delegate, HandlePawnExtension을 호출
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddConfigDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, ChangeContext);

			//AddExtensionHandler : APawn::StaticClass '모든 Pawn'에다가, Extension이 오면 Delegate를 호출하도록 등록.
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(APawn::StaticClass(), AddConfigDelegate);
			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}

}

void UGameFeatureAction_AddInputConfig::HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	/*
	*	* EventName : 
		Experience이동시, PlayerPawn이 아닌, 관전자 Pawn이 설정이 되었다.
		-> Controller로 Pawn을 찾기 때문에 Playercontroller로 설정이 안되어있을 수 있다.
		이는 PlayerPawn에서 Camera와 Input을 설정받는 HeroComponent가 생성이 완료된 시점에서 Pawn을
		찾아야 하나, 그것을 보장받을 수 있도록 HeroComponent의 InitalizePlayerInput이 종료되는 시점(Config가 설정이 완료되는 시점)
		에서 Callback을 받을 수 있도록 Callback을 받을 Pawn과 EventName을 설정해 주어 해당 EventName이 Handle에 들어오면
		Event를 실행시킨다.
	*/
	APawn* AsPawn = CastChecked<APawn>(Actor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || EventName == UCloneHeroComponent::Name_BindInputsNow)
	{
		AddInputConfig(AsPawn, ActiveData);
	}
	else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved || EventName == UCloneHeroComponent::Name_BindInputsNow)
	{
		RemoveInputConfig(AsPawn, ActiveData);
	}
}

void UGameFeatureAction_AddInputConfig::AddInputConfig(APawn* Pawn, FPerContextData& ActiveData)
{
	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (ULocalPlayer* LP = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			FModifyContextOptions Options = { };
			Options.bIgnoreAllPressedKeysUntilRelease = false;

			for (const FCloneMappableConfigPair& Pair : InputConfigs)
			{
				if (Pair.bShouldActivateAutomatically)
				{
					Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
				}
			}

			//Pawn Caching
			ActiveData.PawnsAddedTo.AddUnique(Pawn);
		}
	}
}

void UGameFeatureAction_AddInputConfig::RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData)
{
	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (ULocalPlayer* LP = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for (const FCloneMappableConfigPair& Pair : InputConfigs)
			{
				Subsystem->RemovePlayerMappableConfig(Pair.Config.LoadSynchronous());
			}

			//Caching Pawn Data remove
			ActiveData.PawnsAddedTo.Remove(Pawn);
		}
	}
}

void UGameFeatureAction_AddInputConfig::Reset(FPerContextData& ActiveData)
{
	/*
	* GameFeatureAction은 GameFeature에 종속적 (당연함)
	* GameFeature은 EngineSubsystem임으로,
	* Play in 상황에서 아무리 껏다 키더라도 사라지지 않음으로,
	* 실행하고 끌때마다 Reset을 해주어야한다. 
	*/
	//TArray로 된 ExtensionRequestHandles를 초기화
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.PawnsAddedTo.IsEmpty())
	{
		//Get Pawn in ActiveData
		TWeakObjectPtr<APawn> PawnPtr = ActiveData.PawnsAddedTo.Top();
		//is Exist?
		if (PawnPtr.IsValid())
		{
			RemoveInputConfig(PawnPtr.Get(), ActiveData);
		}
		else
		{
			//WaekObjectPtr이기 때문에 Pop
			ActiveData.PawnsAddedTo.Pop();
		}
	}
}
/*
	Event랑 Delegate랑 다름?..
	
	Delegate : 구독 요청 -> BroadCast시 구독 신청한 모든 이들이게 발동.

	Event : EventName을 통해 특정 Binding된 대상만 발동.
*/