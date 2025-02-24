// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneHeroComponent.h"

#include "CloneProject/CloneLogChannels.h"
#include "CloneProject/CloneGamePlayTags.h"
#include "CloneProject/Character/ClonePawnExtensionComponent.h"
#include "CloneProject/Player/ClonePlayerState.h"
#include "Components/GameFrameworkComponentManager.h"
#include "CloneProject/Character/ClonePawnData.h"

#include "CloneProject/Camera/CloneCameraMode.h"
#include "CloneProject/Camera/CloneCameraComponent.h"
#include "CloneProject/Input/CloneInputComponent.h"
#include "CloneProject/Player/ClonePlayerController.h"
#include "CloneProject/Input/CloneInputConfig.h"
#include "PlayerMappableInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"


const FName UCloneHeroComponent::Name_ActorFeatureName("HeroComponent");

UCloneHeroComponent::UCloneHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UCloneHeroComponent::OnReigster()
{
	Super::OnRegister();

	{

		if (!GetPawn<APawn>()) // OwnerPawn�� �������� �ʴ´ٸ�,
		{
			UE_LOG(LogClone, Error, TEXT("This Component has been added to a BP Whose base class is not a pawn"));
			return;
		}

	}

	//InitState�� ����! FeatureName�� �ʼ�!
	RegisterInitStateFeature();
}

void UCloneHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	//ExtensionComponent�� State�� Ȯ��, bcuz : ExtComponent�� Manager�� ������ �ϱ� ������ �ٸ� Component���� ExtComp���� Ȯ�ΰ���.
	BindOnActorInitStateChanged(UClonePawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	//InitState_Spawned�� �ʱ�ȭ
	ensure(TryToChangeInitState(FCloneGameplayTags::Get().InitState_Spawned));

	//ForceUpdate
	CheckDefaultInitialization();
}

void UCloneHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//�����Ҵ� ó�� Register�� �ߴٸ�, UnRegister�������!
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}


void UCloneHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	/*
		Params's role = Find Other Component through feature name or state 
	*/
	const FCloneGameplayTags& InitTags = FCloneGameplayTags::Get();

	//Find ExtComp
	if (Params.FeatureName == UClonePawnExtensionComponent::NAME_ActorFeatureName)
	{
		//ExtComp's State = Initialized?
		if (Params.FeatureState == InitTags.InitState_DataInitialized)
		{
			//proceed change state to possible
			CheckDefaultInitialization();
		}
	}
}

bool UCloneHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FCloneGameplayTags& InitTags = FCloneGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	AClonePlayerState* ClonePS = GetPlayerState<AClonePlayerState>();

	//Spawned �ʱ�ȭ
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}

	//Spanwed -> DataAvailable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!ClonePS)
		{
			return false;
		}

		return true;
	}

	//DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		/*
			PawnExtComp�� DataInitailized�� �� ����
			Pawn�� �ִ�, ExtComponent::Name��, InitState_Step�϶� InitState�� ����.
			�Լ��̸���� 1. Pawn ������ �ִ� 2. Feature(Component)�� 3.InitState�� �����ߴٸ�, 
		*/
		return ClonePS && Manager->HasFeatureReachedInitState(Pawn, UClonePawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}
	
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}


	return false;
}

void UCloneHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FCloneGameplayTags& InitTags = FCloneGameplayTags::Get();

	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AClonePlayerState* ClonePS = GetPlayerState<AClonePlayerState>();
		if (!ensure(Pawn && ClonePS))
		{
			return;
		}


		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UClonePawnData* PawnData = nullptr;
		if (UClonePawnExtensionComponent* PawnExtComp = UClonePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UClonePawnData>();
		}

		//Camera Handling
		if (bIsLocallyControlled && PawnData)
		{
			if (UCloneCameraComponent* CameraComponent = UCloneCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}

		//Input Handling
		if (AClonePlayerController* ClonePC = GetController<AClonePlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
}

void UCloneHeroComponent::CheckDefaultInitialization()
{
	/*
		ExtComp���� ������ : CheckDefaultInitializationForImplementers()�� ����.
		���� : ���� ������ ��� Component���� CheckDafaultInitailization()ȣ���� �ǹ��� ForImplementers��
		Manager�� ExtComp���Ը� �ʿ��ϱ� ����.
	*/

	const FCloneGameplayTags& InitTags = FCloneGameplayTags::Get();

	static const TArray<FGameplayTag> StateChain = {
		InitTags.InitState_Spawned,
		InitTags.InitState_DataAvailable,
		InitTags.InitState_DataInitialized,
		InitTags.InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}

TSubclassOf<UCloneCameraMode> UCloneHeroComponent::DetermineCameraMode() const
{
	const APawn* Pawn = GetPawn<APawn>();

	//HeroComponent�� Owner Pawn�� ���ٸ� nullptr
	if (!Pawn)
	{
		return nullptr;
	}

	//HeroComponent's OwnerPawn's ExtComp for Get PawnData
	if (UClonePawnExtensionComponent* PawnExtComp = UClonePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		//for Get DefaultCameraMode of PawnClass
		if (const UClonePawnData* PawnData = PawnExtComp->GetPawnData<UClonePawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
		
	}

	return nullptr;
}

void UCloneHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	//Find Pawn
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	//for get local player
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	//for get enhanced input local player subsystem
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem); 

	//Clear SubSystem's MappingContext
	Subsystem->ClearAllMappings();

	if (const UClonePawnExtensionComponent* PawnExtComp = UClonePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UClonePawnData* PawnData = PawnExtComp->GetPawnData<UClonePawnData>())
		{
			if (const UCloneInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FCloneGameplayTags& GameplayTags = FCloneGameplayTags::Get();

				for (const FCloneMappableConfigPair& Pair : DefaultInputConfigs)
				{
					FModifyContextOptions Options = { }; 
					Options.bIgnoreAllPressedKeysUntilRelease = false;

					Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
				}

				UCloneInputComponent* CloneIC = CastChecked<UCloneInputComponent>(PlayerInputComponent);
				{
					CloneIC->BindNatvieAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
					CloneIC->BindNatvieAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
				}
			}
		}
	}
	
}

void UCloneHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		//X : Right, Left(Negative)
		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);

			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		//Y : Forward, Backward(Negative)
		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);

			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
		
}

void UCloneHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		//Yaw = Horizontoal
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		//Pitch = Verical
		double AnimInversionValue = -Value.Y;
		Pawn->AddControllerPitchInput(AnimInversionValue);
	}
}

