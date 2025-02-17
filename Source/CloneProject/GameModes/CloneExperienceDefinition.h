// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CloneExperienceDefinition.generated.h"

class UClonePawnData;
/**
 * 
 */
UCLASS()
class CLONEPROJECT_API UCloneExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UCloneExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<UClonePawnData> DefaultPawnData;


	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;
};

/*
	UserFacingExperience = Type_DataAsset
	PawnDataAsset = Type_DataAsset
	ExperienceDefinition = Type_Blueprint
	
	왜 Definition만 Type이 Blueprint일까?
	그러기 위해서는 CDO(Class Default Object)에 대해 알아야한다.

	우선 CDO는 GetDefault<CastingObject>(Class);를 통해 가져올 수 있다. 

	UClass로 Static Class를 만들 수 있다. = MetaData를 만들 수 있다.

	CDO는 UClass의 초기화 값으로 이루어진 객체화된 Instance이다.

	Q. 그렇다면 Unreal에서는 왜 이런 것을 지원하는가?
	A. 델타 시리얼라이제이션을 지원하기 위함이다. 

	EX. 기본값을 가진 Instnace와, 값이 변경된 Instance들이 있을 때.
	기본값을 기준으로 다른 값들의 Data를 저장하는 것이 Dela Serialization이라고 한다.
	
	EX2. 여러 Actor들이 소환이 되었을 때, 몇 몇의 Instance의 값이 변경이 필요하다면,
	Dela Serialization에서 변경된 값을 찾아 넣어준다. 나머지는 Default값과 동일하다면,
	그대로 사용한다. Memory의 효율관련된 것이다.

	Result = Class Default Object(CDO)란, UClass의 기본 값으로 저장된 Instance이다.
		필요한 이유는 CDO와 비교하여 값이 바뀐 Instance가 존재한다면, Delta Serialization을 통해 변경하여 
		Memory적으로 이득을 보고, Network상에서도 바뀐부분만 전송하면 됨으로, 대역폭적으로도 이점이 있다.

	StaticClass = MetaData의 집합
	CDO = Default값으로 이루어진 Instance(NewObject)

	CDO는 Object(Instance)들의 기준점이 되기 때문에 값이 변경되지 않도록 조심히 다루어야 해서 Const를 붙혀서 사용하는것이 바람직하다.
*/

/*
*	--PrimaryDataAsset과 CDO의 관계--
* 
	PrimaryAsset은 DataAsset을 상속받아 DataAsset을 편리하게 사용할 수 있게 하는 Class이다.

	PrimaryAsset에 GetPrimaryAssetId()함수가 존재하는데, 이 함수 내부에서는 CDO를 판별하고,
	CDO를 받아서 주지 않는다면, Type을 그대로 반환하게 된다.

	문제가 되는 부분은 PrimaryAssetId를 설정했을 때 (Type, Name)을 제대로 설정하지 않는다면 상속받은 Blueprint Class를 그대로 사용하게 된다.

	AssetManager에서 Scan을 진행할 때, 모든 ExperienceDefinition에 대해 Scan이 진행되어야 하지만, Type설정에 실패시 Blueprint class만
	Scan하기 때문에 다른 Experience를 Scan할 수 없게 된다. 

	이러한 것을 방지하기위해 사실 GetPrimaryAssetId는 CDO여야만 작동하게 되어있다.
	GetPrimaryAssetId는 GetSuperClass를 통해 부모 Class를 타고 쭉 올라오며, C++로만 이루어진 UClass를 찾는다면,
	해당 Class를 반환한다.
	
	Q. ExperienceDefinition을 DataAsset으로 만들지 않는 이유
	A. Editor 내부에서 DA_ExperienceDefinition으로 만든다면, 타고 올라갈 SuperClass가 없기 때문에 Scan을 할 수 없다.
*/