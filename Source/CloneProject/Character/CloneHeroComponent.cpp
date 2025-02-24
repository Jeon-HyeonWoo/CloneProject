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

		if (!GetPawn<APawn>()) // OwnerPawn이 존재하지 않는다면,
		{
			UE_LOG(LogClone, Error, TEXT("This Component has been added to a BP Whose base class is not a pawn"));
			return;
		}

	}

	//InitState를 부착! FeatureName은 필수!
	RegisterInitStateFeature();
}

void UCloneHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	//ExtensionComponent의 State만 확인, bcuz : ExtComponent는 Manager의 역할을 하기 때문에 다른 Component들은 ExtComp에서 확인가능.
	BindOnActorInitStateChanged(UClonePawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	//InitState_Spawned로 초기화
	ensure(TryToChangeInitState(FCloneGameplayTags::Get().InitState_Spawned));

	//ForceUpdate
	CheckDefaultInitialization();
}

void UCloneHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//동적할당 처럼 Register를 했다면, UnRegister해줘야함!
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

	//Spawned 초기화
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
			PawnExtComp가 DataInitailized일 때 승인
			Pawn에 있는, ExtComponent::Name이, InitState_Step일때 InitState를 진행.
			함수이름대로 1. Pawn 가지고 있는 2. Feature(Component)가 3.InitState에 도달했다면, 
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
		ExtComp와의 차이점 : CheckDefaultInitializationForImplementers()의 부재.
		이유 : 나를 제외한 모든 Component들을 CheckDafaultInitailization()호출의 의미인 ForImplementers는
		Manager인 ExtComp에게만 필요하기 때문.
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

	//HeroComponent가 Owner Pawn이 없다면 nullptr
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

