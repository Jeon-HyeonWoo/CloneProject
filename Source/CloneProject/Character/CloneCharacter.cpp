// Fill out your copyright notice in the Description page of Project Settings.


#include "CloneCharacter.h"

#include "CloneProject/Character/ClonePawnExtensionComponent.h"
#include "CloneProject/Camera/CloneCameraComponent.h"

// Sets default values
ACloneCharacter::ACloneCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	
	{
		//Create PawnExtensionComponent
		PawnExtComponent = CreateDefaultSubobject<UClonePawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	}
	
	{
		//Create CameraComponent
		CameraComponent = CreateDefaultSubobject<UCloneCameraComponent>(TEXT("CameraComponent"));
		CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	}
	
}

// Called when the game starts or when spawned
void ACloneCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACloneCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACloneCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();

}

