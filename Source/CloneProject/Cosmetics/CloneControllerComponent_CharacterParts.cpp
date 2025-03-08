
#include "CloneControllerComponent_CharacterParts.h"
#include "ClonePawnComponent_CharacterParts.h"

UCloneControllerComponent_CharacterParts::UCloneControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UCloneControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (AController* OwingController = GetController<AController>())
		{
			//Possessed 하고 있는 Pawn이 변경 될 때 마다 설정되는 Delegate
			OwingController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
		}
	}
}

void UCloneControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();
	Super::EndPlay(EndPlayReason);
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

void UCloneControllerComponent_CharacterParts::RemoveAllCharacterParts()
{
	if (UClonePawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (FCloneControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}
	CharacterParts.Reset();
}

void UCloneControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	//OldPawn이 존재하면? OldPawn을 가져와서 전부 Remove
	if (UClonePawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<UClonePawnComponent_CharacterParts>() : nullptr)
	{
		for (FCloneControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
			
		}
	}

	if (UClonePawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<UClonePawnComponent_CharacterParts>() : nullptr)
	{
		for (FCloneControllerCharacterPartEntry& Entry : CharacterParts)
		{
			check(!Entry.Handle.IsValid());
			Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
		}
	}
}
