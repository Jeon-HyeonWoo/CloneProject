// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CloneUserFacingExperience.generated.h"

/**
 * 
 */
UCLASS()
class CLONEPROJECT_API UCloneUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	/* the Specific map to load */
	/*
		AllowedTypes : "TEXT" TEXT�� ���õ� ������ ���� �� �ִ�.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/* the gameplay experience to load */
	/*
		
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "CloneExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};
/*
	- Exerpience : ������ ���������� �����ϴ� ��(Mode), �κ� ���� ��� �� �پ��ϰ� ���� �� �ִ�.
	- AssetManager�� PrimaryAsset�� ���� Loading�� Unloading�� �ϱ� ������, 
	Editor���� DataAsset(imple_UPrimaryDataAsset)�� Load, Unload�� �� �ִ�.
*/

/*
	UPrimaryDataAsset�� ��ӹ��� DA_DataAsset�� ����ϴ���, �ش� ������ Ž���� �� �� ����.
	�� ������ AssetManager�� �� �� ���� �����̴�.
	���� AssetManager���� �ش� �������� ã�� �� �ְԲ�(Scan) ���־�� �Ѵ�. (Editor_Project Setting ������)
	AssetManager���� Load�� FSoftObjectPtr�� ����ϴµ�, �̴� Object�� ��� ���� �ƴ� ���(Path)�� �˰� �ִ� �����̱� �����̴�.
	ProjectSetting���� Scan�� ���� ���� �ش� ��θ� �����ϰ� �־�� ��� �ǹ��̴�.

	Scan ����� ProjectSetting_AssetManager_PrimaryAssetTypeToScan_���� �ش� ��ο� ���� ��� �� Ȥ�� Ư�� ������ �����Ͽ� ������ �� �ִ�.
*/

/*
	** Experience Loading
	* 
	* Make, BP_ExperienceList3D, BP_TeleportToUserFacingExperience
	* BP_ExperrienceList3D : ASyncLoad Experience -> Spawn_BP_TeleportToUserFacingExperience
	* BP_TeleportToUserFacingExperience : this objct make user teleport to other Experience(Map and Mode);
*/