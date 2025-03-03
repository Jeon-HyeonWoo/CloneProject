
#include "CloneControllerComponent_CharacterParts.h"
#include "ClonePawnComponent_CharacterParts.h"

UCloneControllerComponent_CharacterParts::UCloneControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UClonePawnComponent_CharacterParts* UCloneControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	//GetPawn : Controller에 부착된 Pawn을 가져옴
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
	//AddDefault_GetRef : 공간할당 + 주소 값 넘김 = 복사 연산 x
	FCloneControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();

	NewEntry.Part = NewPart;

	if (UClonePawnComponent_CharacterParts * PawnCustomizer = GetPawnCustomizer())
	{
		NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
	}
}
