// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerLevelSelect.h"

// Sets default values
AMyPlayerLevelSelect::AMyPlayerLevelSelect()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	playerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Player static mesh"));

	cameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");

	cameraAttach = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	//setups the camera to follow the player
	cameraAttach->SetupAttachment(playerMesh);
	cameraComponent->AttachToComponent(cameraAttach, FAttachmentTransformRules::KeepRelativeTransform);
	cameraAttach->bUsePawnControlRotation = true;
	cameraAttach->bEnableCameraLag = false;

	RootComponent = playerMesh;
}

// Called when the game starts or when spawned
void AMyPlayerLevelSelect::BeginPlay()
{
	Super::BeginPlay();
	playerMesh->SetSimulatePhysics(true);
}

// Called every frame
void AMyPlayerLevelSelect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPlayerLevelSelect::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Horizontal", this, &AMyPlayerLevelSelect::MoveHorizontal);
	PlayerInputComponent->BindAxis("Vertical", this, &AMyPlayerLevelSelect::MoveVertical);

}

//moves the player horizontally
void AMyPlayerLevelSelect::MoveHorizontal(float horizontal)
{
	playerMesh->SetAllPhysicsLinearVelocity(FVector(playerMoveSpeed * -horizontal, playerMesh->GetPhysicsLinearVelocity().Y, 0), false);
}

//moves the player vertically
void AMyPlayerLevelSelect::MoveVertical(float vertical)
{
	playerMesh->SetAllPhysicsLinearVelocity(FVector(playerMesh->GetPhysicsLinearVelocity().X, playerMoveSpeed * vertical, 0), false);
}