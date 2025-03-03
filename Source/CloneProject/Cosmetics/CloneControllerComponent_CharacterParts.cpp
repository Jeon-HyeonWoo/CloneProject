
#include "CloneControllerComponent_CharacterParts.h"
#include "ClonePawnComponent_CharacterParts.h"

UCloneControllerComponent_CharacterParts::UCloneControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UClonePawnComponent_CharacterParts* UCloneControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	//GetPawn : Controller�� ������ Pawn�� ������
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		return ControlledPawn->FindComponentByClass<UClonePawnComponent_CharacterParts>();
	}
	return nullptr;
}

void UCloneControllerComponent_CharacterParts::AddCharacterPart(const FCloneCharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart);
}

void UCloneControllerComponent_CharacterParts::AddCharacterPartInternal(const FCloneCharacterPart& NewPart)
{
	//AddDefault_GetRef : �����Ҵ� + �ּ� �� �ѱ� = ���� ���� x
	FCloneControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();

	NewEntry.Part = NewPart;

	if (UClonePawnComponent_CharacterParts * PawnCustomizer = GetPawnCustomizer())
	{
		NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
	}
}
