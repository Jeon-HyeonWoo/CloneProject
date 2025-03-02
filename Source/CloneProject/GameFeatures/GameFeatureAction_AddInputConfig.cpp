#include "GameFeatureAction_AddInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
#include "Components/GameFrameworkComponentManager.h"
#include "CloneProject/Character/CloneHeroComponent.h"

void UGameFeatureAction_AddInputConfig::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	//Ȥ�� �ʱ�ȭ�� ���� �ʾҳ� �� �� �� üũ �� ���� WorldActionBase�� Activating ����
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
			//�߰����� ��û�� ���� Delegate, HandlePawnExtension�� ȣ��
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddConfigDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, ChangeContext);

			//AddExtensionHandler : APawn::StaticClass '��� Pawn'���ٰ�, Extension�� ���� Delegate�� ȣ���ϵ��� ���.
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(APawn::StaticClass(), AddConfigDelegate);
			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}

}

void UGameFeatureAction_AddInputConfig::HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	/*
	*	* EventName : 
		Experience�̵���, PlayerPawn�� �ƴ�, ������ Pawn�� ������ �Ǿ���.
		-> Controller�� Pawn�� ã�� ������ Playercontroller�� ������ �ȵǾ����� �� �ִ�.
		�̴� PlayerPawn���� Camera�� Input�� �����޴� HeroComponent�� ������ �Ϸ�� �������� Pawn��
		ã�ƾ� �ϳ�, �װ��� ������� �� �ֵ��� HeroComponent�� InitalizePlayerInput�� ����Ǵ� ����(Config�� ������ �Ϸ�Ǵ� ����)
		���� Callback�� ���� �� �ֵ��� Callback�� ���� Pawn�� EventName�� ������ �־� �ش� EventName�� Handle�� ������
		Event�� �����Ų��.
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
	* GameFeatureAction�� GameFeature�� ������ (�翬��)
	* GameFeature�� EngineSubsystem������,
	* Play in ��Ȳ���� �ƹ��� ���� Ű���� ������� ��������,
	* �����ϰ� �������� Reset�� ���־���Ѵ�. 
	*/
	//TArray�� �� ExtensionRequestHandles�� �ʱ�ȭ
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
			//WaekObjectPtr�̱� ������ Pop
			ActiveData.PawnsAddedTo.Pop();
		}
	}
}
/*
	Event�� Delegate�� �ٸ�?..
	
	Delegate : ���� ��û -> BroadCast�� ���� ��û�� ��� �̵��̰� �ߵ�.

	Event : EventName�� ���� Ư�� Binding�� ��� �ߵ�.
*/