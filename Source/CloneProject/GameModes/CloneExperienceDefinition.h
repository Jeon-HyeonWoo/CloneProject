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
	
	�� Definition�� Type�� Blueprint�ϱ�?
	�׷��� ���ؼ��� CDO(Class Default Object)�� ���� �˾ƾ��Ѵ�.

	�켱 CDO�� GetDefault<CastingObject>(Class);�� ���� ������ �� �ִ�. 

	UClass�� Static Class�� ���� �� �ִ�. = MetaData�� ���� �� �ִ�.

	CDO�� UClass�� �ʱ�ȭ ������ �̷���� ��üȭ�� Instance�̴�.

	Q. �׷��ٸ� Unreal������ �� �̷� ���� �����ϴ°�?
	A. ��Ÿ �ø���������̼��� �����ϱ� �����̴�. 

	EX. �⺻���� ���� Instnace��, ���� ����� Instance���� ���� ��.
	�⺻���� �������� �ٸ� ������ Data�� �����ϴ� ���� Dela Serialization�̶�� �Ѵ�.
	
	EX2. ���� Actor���� ��ȯ�� �Ǿ��� ��, �� ���� Instance�� ���� ������ �ʿ��ϴٸ�,
	Dela Serialization���� ����� ���� ã�� �־��ش�. �������� Default���� �����ϴٸ�,
	�״�� ����Ѵ�. Memory�� ȿ�����õ� ���̴�.

	Result = Class Default Object(CDO)��, UClass�� �⺻ ������ ����� Instance�̴�.
		�ʿ��� ������ CDO�� ���Ͽ� ���� �ٲ� Instance�� �����Ѵٸ�, Delta Serialization�� ���� �����Ͽ� 
		Memory������ �̵��� ����, Network�󿡼��� �ٲ�κи� �����ϸ� ������, �뿪�������ε� ������ �ִ�.

	StaticClass = MetaData�� ����
	CDO = Default������ �̷���� Instance(NewObject)

	CDO�� Object(Instance)���� �������� �Ǳ� ������ ���� ������� �ʵ��� ������ �ٷ��� �ؼ� Const�� ������ ����ϴ°��� �ٶ����ϴ�.
*/

/*
*	--PrimaryDataAsset�� CDO�� ����--
* 
	PrimaryAsset�� DataAsset�� ��ӹ޾� DataAsset�� ���ϰ� ����� �� �ְ� �ϴ� Class�̴�.

	PrimaryAsset�� GetPrimaryAssetId()�Լ��� �����ϴµ�, �� �Լ� ���ο����� CDO�� �Ǻ��ϰ�,
	CDO�� �޾Ƽ� ���� �ʴ´ٸ�, Type�� �״�� ��ȯ�ϰ� �ȴ�.

	������ �Ǵ� �κ��� PrimaryAssetId�� �������� �� (Type, Name)�� ����� �������� �ʴ´ٸ� ��ӹ��� Blueprint Class�� �״�� ����ϰ� �ȴ�.

	AssetManager���� Scan�� ������ ��, ��� ExperienceDefinition�� ���� Scan�� ����Ǿ�� ������, Type������ ���н� Blueprint class��
	Scan�ϱ� ������ �ٸ� Experience�� Scan�� �� ���� �ȴ�. 

	�̷��� ���� �����ϱ����� ��� GetPrimaryAssetId�� CDO���߸� �۵��ϰ� �Ǿ��ִ�.
	GetPrimaryAssetId�� GetSuperClass�� ���� �θ� Class�� Ÿ�� �� �ö����, C++�θ� �̷���� UClass�� ã�´ٸ�,
	�ش� Class�� ��ȯ�Ѵ�.
	
	Q. ExperienceDefinition�� DataAsset���� ������ �ʴ� ����
	A. Editor ���ο��� DA_ExperienceDefinition���� ����ٸ�, Ÿ�� �ö� SuperClass�� ���� ������ Scan�� �� �� ����.
*/