// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneCosmeticAnimationTypes.h"

USkeletalMesh* FCloneAnimBodyStyleSelectionSet::SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const
{
    for (const FCloneAnimBodyStyleSelectionEntry& Rule : MeshRules)
    {
        //Rule�� Mesh�� �����ϰ�, && CosmeticTags�� Rule�� RequiredTags�� '�Ϻ��ϰ� ��ġ(has all)'�Ѵٸ�,
        if ((Rule.Mesh) && CosmeticTags.HasAll(Rule.RequiredTags))
        {
            return Rule.Mesh;
        }
    }
    return DefaultMesh;
}
