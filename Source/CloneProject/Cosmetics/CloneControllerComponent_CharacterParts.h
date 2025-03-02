#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "CloneCharacterPartTypes.h"
#include "CloneControllerComponent_CharacterParts.generated.h"


USTRUCT()
struct FCloneControllerCharacterPartEntry
{
	GENERATED_BODY()

	/* Character Part�� ���� ����(Meta Data) */
	UPROPERTY(EditAnywhere)
	FCloneCharacterPart Part;

	/* character part �ڵ� (������) - Controller�� Possess�ϰ� �ִ� Pawn���� ������(Instance) Character Part Handle�� */
	/* �ش� Component�� ������ ���� �� Part�� ������ȣ ��� �����ϸ� ����. */
	FCloneCharacterPartHandle Handle;
};

UCLASS(meta = (BlueprintSpawnableComponent))
class UCloneControllerComponent_CharacterParts : public UControllerComponent
{
	GENERATED_BODY()

public:

	UCloneControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Category = Cosmetics)
	TArray<FCloneControllerCharacterPartEntry> CharacterParts;
};

/* 
*	1. ControllerComponent�� ��� �޾ұ� ������, Controller�� �����Ѵ�.
*	2. Meta Data�� ���� ������ ���� �͵鸸 ���ϰ� �ִ�.
*	3. ControllerPart�� PawnPart���� Part������ ��û�Ѵ�. 
*	4. ��û�� Part�� PawnPart�� FCloneCharacterPartList�� �����Ѵ�. 
*	5. Array�� �׸� �����ϴ� ��ҷ� �и��ؾ� network�󿡼� ���� ���� ������ �̷��� ���¸� ����
*/