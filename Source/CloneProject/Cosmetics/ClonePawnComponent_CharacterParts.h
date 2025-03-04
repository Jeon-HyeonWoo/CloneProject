#pragma once

#include "CoreMinimal.h"
#include "CloneCharacterPartTypes.h"
#include "CloneCosmeticAnimationTypes.h"
#include "Components/PawnComponent.h"
#include "ClonePawnComponent_CharacterParts.generated.h"
#include "D:/Unreal/UE_5.4/Engine/Plugins/Animation/RigLogic/Source/RigLogicLib/Public/dna/layers/Geometry.h"

class UClonePawnComponent_CharacterParts;

/* �ν��Ͻ�ȭ �� Character Part�� ����*/
USTRUCT()
struct FCloneAppliedCharacterPartEntry
{
	GENERATED_BODY()

	/* Character Part�� ����(Meta Data) */
	UPROPERTY()
	FCloneCharacterPart Part;

	/* CloneCharacterPartList���� �Ҵ� ���� Part�� Handle�� 
		FCloneControllerCharacterPartEntry�� Handle���� ���ƾ���
		-> ������ ���� Part
	*/
	UPROPERTY()
	int32 PartHandle = INDEX_NONE;

	/* �ν��Ͻ�ȭ�� Character Part�� Actor - Caching�� �س��� Ż������ �����ϱ� �����ϰ�. */
	/* ChildComponent : Actor���� Actor�� ������ų ���� childActorcomponent�� ����Ͽ� ���� */
	UPROPERTY()
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

/* ClonePawnComponent_CharacterParts���� ������ Character Parts�� �����ϴ� Ŭ���� */
USTRUCT(BlueprintType)
struct FCloneCharacterPartList
{
	GENERATED_BODY()

	FCloneCharacterPartList()
		: OwnerComponent(nullptr)
	{}

	FCloneCharacterPartList(UClonePawnComponent_CharacterParts* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	bool SpawnActorForEntry(FCloneAppliedCharacterPartEntry& Entry);

	FCloneCharacterPartHandle AddEntry(FCloneCharacterPart NewPart);

	/* ���� �ν��Ͻ�ȭ �� Character Part */
	UPROPERTY()
	TArray<FCloneAppliedCharacterPartEntry> Entries;

	/* �ش� CloneCharacterPartList�� Owner�� PawnComponent */
	UPROPERTY()
	TObjectPtr<UClonePawnComponent_CharacterParts> OwnerComponent;
	
	/* PartHandle�� ���� �Ҵ� �� �����ϴ� ���� */
	int32 PartHandleCounter = 0;
};

UCLASS()
class CLONEPROJECT_API UClonePawnComponent_CharacterParts : public UPawnComponent
{
	GENERATED_BODY()
public:

	UClonePawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	USkeletalMeshComponent* GetParentMeshComponent() const;
	USceneComponent* GetSceneComponentToAttachTo() const;
	void BroadcastChagned();

	FCloneCharacterPartHandle AddCharacterPart(const FCloneCharacterPart& NewPart);

	/* Instanceȭ �� CharacterParts */
	UPROPERTY()
	FCloneCharacterPartList CharacterPartList;


	/* Animation ������ ���� Mesh�� ����� */
	UPROPERTY(EditAnywhere, Category = Cosmetics)
	FCloneAnimBodyStyleSelectionSet BodyMeshes;
};
