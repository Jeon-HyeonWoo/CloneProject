
#include "ClonePawnComponent_CharacterParts.h"
#include "GameFramework/Character.h"

bool FCloneCharacterPartList::SpawnActorForEntry(FCloneAppliedCharacterPartEntry& Entry)
{
	bool bCreatedAnyActor = false;

	//if Meta Data is nullptr 
	if (Entry.Part.PartClass != nullptr)
	{
		//has been ownercomp world
		UWorld* World = OwnerComponent->GetWorld();

		//ownerComponent's Scene = PawnCompnentPart's Character(Owner) Owner to Owner
		if (USceneComponent* ComponentToAttachTo = OwnerComponent->GetSceneComponentToAttachTo())
		{
			const FTransform SpawnTransform = ComponentToAttachTo->GetSocketTransform(Entry.Part.SocketName);

			//for Attach to Actor To Actor (OwnerComponent : PawnPart, GetOwner() : Possess Pawn)
			UChildActorComponent* PartComponent = NewObject<UChildActorComponent>(OwnerComponent->GetOwner());
			//Pawn's SceneComponent attach, if socket name == none : RootComponent
			PartComponent->SetupAttachment(ComponentToAttachTo, Entry.Part.SocketName);

			/*
				SetChildActorClass는 GameThread에서 실행 -> World에 등록이 되지 않았기 때문에 Rendering이 안됨.
				RegisterComponent는 GameThread와 Render/Physics Thread와 동기화를 진행.
				Scene Graph에 등록하여 Rendering이 가능하게 함.
				비유 : 
				- SetChildActorClass는 Disk에 정보를 담아둔 상태. 
				- RegisterComponent는 Memory에 올리고(Component 명시적 등록) Cpu에서 계산(Render)하는 과정.
			*/
			PartComponent->SetChildActorClass(Entry.Part.PartClass);
			PartComponent->RegisterComponent();

			//GetChildActor = NewObject_Actor
			if (AActor* SpawnActor = PartComponent->GetChildActor())
			{
				if (USceneComponent* SpawnedRootComponent = SpawnActor->GetRootComponent())
				{
					//PartComponent의 OwnerActor보다 Tick이 먼저 활성화 될 것을 우려하여 사용함.
					//따라서 SpawnedRootComponent(PartActor)가 ComponentToActtachTo(OwnerAcotr)보다 늦게 실행되도록 보장해주세요. 함수
					SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
				}
			}

			//Cache Data
			Entry.SpawnedComponent = PartComponent;
			bCreatedAnyActor = true;
		}
	}

	return bCreatedAnyActor;
}

FCloneCharacterPartHandle FCloneCharacterPartList::AddEntry(FCloneCharacterPart NewPart)
{
	//Create Handle
	FCloneCharacterPartHandle Result;
	Result.PartHandle = PartHandleCounter++;

	if (ensure(OwnerComponent && OwnerComponent->GetOwner() && OwnerComponent->GetOwner()->HasAuthority()))
	{
		FCloneAppliedCharacterPartEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Part = NewPart;
		NewEntry.PartHandle = Result.PartHandle;

		if (SpawnActorForEntry(NewEntry))
		{
			OwnerComponent->BroadcastChagned();
		}
	}

	return FCloneCharacterPartHandle();
}


UClonePawnComponent_CharacterParts::UClonePawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USkeletalMeshComponent* UClonePawnComponent_CharacterParts::GetParentMeshComponent() const
{
	//해당 Class는 PawnCompont을 상속받았기 때문에 Owner는 PawnComponent가 부착되어있는 Pawn
	if (AActor* OwnerActor = GetOwner())
	{
		if (ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor))
		{
			if (USkeletalMeshComponent* MeshComponent = OwningCharacter->GetMesh())
			{
				return MeshComponent;
			}
		}
	}
}

USceneComponent* UClonePawnComponent_CharacterParts::GetSceneComponentToAttachTo() const
{

	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		return MeshComponent;
	}
	else if (AActor* OwnerActor = GetOwner())	//혹시나 하는 Root Component확인
	{
		return OwnerActor->GetRootComponent();
	}

	return nullptr;
}

void UClonePawnComponent_CharacterParts::BroadcastChagned()
{
	const bool bReinitPose = true;

	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		const FGameplayTagContainer MergedTags = GetCombinedTags(FGameplayTag());
		USkeletalMesh* DesiredMesh = BodyMeshes.SelectBestBodyStyle(MergedTags);

		MeshComponent->SetSkeletalMesh(DesiredMesh, bReinitPose);

		if (UPhysicsAsset* PhysicsAsset = BodyMeshes.ForcedPhysicsAsset)
		{
			MeshComponent->SetPhysicsAsset(PhysicsAsset, bReinitPose);
		}
	}
}

FCloneCharacterPartHandle UClonePawnComponent_CharacterParts::AddCharacterPart(const FCloneCharacterPart& NewPart)
{
	return CharacterPartList.AddEntry(NewPart);
}


