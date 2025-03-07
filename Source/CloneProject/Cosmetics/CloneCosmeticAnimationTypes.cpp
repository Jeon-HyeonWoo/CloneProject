// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneCosmeticAnimationTypes.h"

USkeletalMesh* FCloneAnimBodyStyleSelectionSet::SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const
{
    for (const FCloneAnimBodyStyleSelectionEntry& Rule : MeshRules)
    {
        //Rule의 Mesh가 존재하고, && CosmeticTags와 Rule의 RequiredTags와 '완벽하게 일치(has all)'한다면,
        if ((Rule.Mesh) && CosmeticTags.HasAll(Rule.RequiredTags))
        {
            return Rule.Mesh;
        }
    }
    return DefaultMesh;
}
