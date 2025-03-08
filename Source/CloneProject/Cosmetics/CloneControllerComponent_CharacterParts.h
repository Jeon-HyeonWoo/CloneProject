#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "CloneCharacterPartTypes.h"
#include "CloneControllerComponent_CharacterParts.generated.h"

class UClonePawnComponent_CharacterParts;

USTRUCT()
struct FCloneControllerCharacterPartEntry
{
	GENERATED_BODY()

	/* Character Part에 대한 정의(Meta Data) */
	UPROPERTY(EditAnywhere)
	FCloneCharacterPart Part;

	/* character part 핸들 (고유값) - Controller가 Possess하고 있는 Pawn에서 생성한(Instance) Character Part Handle값 */
	/* 해당 Component의 관리를 위한 각 Part별 고유번호 라고 생각하면 편함. */
	FCloneCharacterPartHandle Handle;
};

UCLASS(meta = (BlueprintSpawnableComponent))
class UCloneControllerComponent_CharacterParts : public UControllerComponent
{
	GENERATED_BODY()

public:

	UCloneControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UClonePawnComponent_CharacterParts* GetPawnCustomizer() const;

	UFUNCTION(BlueprintCallable, Category = Cosmetic)
	void AddCharacterPart(const FCloneCharacterPart& NewPart);
	void AddCharacterPartInternal(const FCloneCharacterPart& NewPart);
	void RemoveAllCharacterParts();

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UPROPERTY(EditAnywhere, Category = Cosmetics)
	TArray<FCloneControllerCharacterPartEntry> CharacterParts;
};

/* 
*	1. ControllerComponent를 상속 받았기 때문에, Controller에 부착한다.
*	2. Meta Data와 같은 무겁지 않은 것들만 지니고 있다.
*	3. ControllerPart는 PawnPart에게 Part부착을 요청한다. 
*	4. 요청된 Part는 PawnPart의 FCloneCharacterPartList에 저장한다. 
*	5. Array와 그를 구성하는 요소로 분리해야 network상에서 득을 보기 때문에 이러한 형태를 지님
*/