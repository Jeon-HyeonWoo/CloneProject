
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
				SetChildActorClass�� GameThread���� ���� -> World�� ����� ���� �ʾұ� ������ Rendering�� �ȵ�.
				RegisterComponent�� GameThread�� Render/Physics Thread�� ����ȭ�� ����.
				Scene Graph�� ����Ͽ� Rendering�� �����ϰ� ��.
				���� : 
				- SetChildActorClass�� Disk�� ������ ��Ƶ� ����. 
				- RegisterComponent�� Memory�� �ø���(Component ����� ���) Cpu���� ���(Render)�ϴ� ����.
			*/
			PartComponent->SetChildActorClass(Entry.Part.PartClass);
			PartComponent->RegisterComponent();

			//GetChildActor = NewObject_Actor
			if (AActor* SpawnActor = PartComponent->GetChildActor())
			{
				if (USceneComponent* SpawnedRootComponent = SpawnActor->GetRootComponent())
				{
					//PartComponent�� OwnerActor���� Tick�� ���� Ȱ��ȭ �� ���� ����Ͽ� �����.
					//���� SpawnedRootComponent(PartActor)�� ComponentToActtachTo(OwnerAcotr)���� �ʰ� ����ǵ��� �������ּ���. �Լ�
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
	//�ش� Class�� PawnCompont�� ��ӹ޾ұ� ������ Owner�� PawnComponent�� �����Ǿ��ִ� Pawn
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
	else if (AActor* OwnerActor = GetOwner())	//Ȥ�ó� �ϴ� Root ComponentȮ��
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


