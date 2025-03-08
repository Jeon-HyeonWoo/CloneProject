#pragma once

#include "CoreMinimal.h"
#include "CloneCharacterPartTypes.h"
#include "CloneCosmeticAnimationTypes.h"
#include "Components/PawnComponent.h"
#include "ClonePawnComponent_CharacterParts.generated.h"

class UClonePawnComponent_CharacterParts;

/* 인스턴스화 된 Character Part의 단위*/
USTRUCT()
struct FCloneAppliedCharacterPartEntry
{
	GENERATED_BODY()

	/* Character Part의 정의(Meta Data) */
	UPROPERTY()
	FCloneCharacterPart Part;

	/* CloneCharacterPartList에서 할당 받은 Part의 Handle값 
		FCloneControllerCharacterPartEntry의 Handle값과 같아야함
		-> 같으면 같은 Part
	*/
	UPROPERTY()
	int32 PartHandle = INDEX_NONE;

	/* 인스턴스화된 Character Part용 Actor - Caching을 해놔서 탈부착과 관리하기 용이하게. */
	/* ChildComponent : Actor에게 Actor를 부착시킬 때는 childActorcomponent를 사용하여 부착 */
	UPROPERTY()
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

/* ClonePawnComponent_CharacterParts에서 실질적 Character Parts를 관리하는 클래스 */
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
	void DestroyActorForEntry(FCloneAppliedCharacterPartEntry& Entry);

	FCloneCharacterPartHandle AddEntry(FCloneCharacterPart NewPart);
	void RemoveEntry(FCloneCharacterPartHandle Handle);

	FGameplayTagContainer CollectCombinedTags() const;

	/* 현재 인스턴스화 된 Character Part */
	UPROPERTY()
	TArray<FCloneAppliedCharacterPartEntry> Entries;

	/* 해당 CloneCharacterPartList의 Owner인 PawnComponent */
	UPROPERTY()
	TObjectPtr<UClonePawnComponent_CharacterParts> OwnerComponent;
	
	/* PartHandle의 값을 할당 및 관리하는 변수 */
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
	FGameplayTagContainer GetCombinedTags(FGameplayTag RequiredPrefix) const;
	void BroadcastChagned();

	FCloneCharacterPartHandle AddCharacterPart(const FCloneCharacterPart& NewPart);
	void RemoveCharacterPart(FCloneCharacterPartHandle Handle);

	/* Instance화 된 CharacterParts */
	UPROPERTY()
	FCloneCharacterPartList CharacterPartList;


	/* Animation 적용을 위한 Mesh와 연결고리 */
	UPROPERTY(EditAnywhere, Category = Cosmetics)
	FCloneAnimBodyStyleSelectionSet BodyMeshes;
};
