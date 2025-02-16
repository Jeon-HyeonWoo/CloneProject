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
		AllowedTypes : "TEXT" TEXT에 관련된 정보만 담을 수 있다.
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
	- Exerpience : 유저가 직접적으로 경험하는 곳(Mode), 로비나 게임 모드 등 다양하게 나눌 수 있다.
	- AssetManager는 PrimaryAsset에 대한 Loading과 Unloading을 하기 때문에, 
	Editor내의 DataAsset(imple_UPrimaryDataAsset)을 Load, Unload할 수 있다.
*/

/*
	UPrimaryDataAsset을 상속받은 DA_DataAsset를 상속하더라도, 해당 변수는 탐색을 할 수 없다.
	그 이유는 AssetManager가 볼 수 없기 때문이다.
	따라서 AssetManager에게 해당 변수들을 찾을 수 있게끔(Scan) 해주어야 한다. (Editor_Project Setting 내에서)
	AssetManager에서 Load시 FSoftObjectPtr를 사용하는데, 이는 Object의 모든 것이 아닌 경로(Path)만 알고 있는 상태이기 때문이다.
	ProjectSetting에서 Scan은 위와 같이 해당 경로를 인지하고 있어라 라는 의미이다.

	Scan 방법은 ProjectSetting_AssetManager_PrimaryAssetTypeToScan_에서 해당 경로에 대한 모든 것 혹은 특정 에셋을 지정하여 설정할 수 있다.
*/

/*
	** Experience Loading
	* 
	* Make, BP_ExperienceList3D, BP_TeleportToUserFacingExperience
	* BP_ExperrienceList3D : ASyncLoad Experience -> Spawn_BP_TeleportToUserFacingExperience
	* BP_TeleportToUserFacingExperience : this objct make user teleport to other Experience(Map and Mode);
*/