// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneHeroComponent.h"

UCloneHeroComponent::UCloneHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}
