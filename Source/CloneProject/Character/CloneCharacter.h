// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "CloneCharacter.generated.h"

class UClonePawnExtensionComponent;
class UCloneCameraComponent;

UCLASS()
class CLONEPROJECT_API ACloneCharacter : public AModularCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACloneCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clone|Character")
	TObjectPtr<UClonePawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clone|Character")
	TObjectPtr<UCloneCameraComponent> CameraComponent;
};
