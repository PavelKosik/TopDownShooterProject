// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
//#include "MyProjectile.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
//#include "MyAIController.h"
//#include "MyHUD.h"
//#include "MyDashStaminaBar.h"
//#include "MyGameInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyPlayer.generated.h"

class MyProjectile;
class MyAIController;
class MyHUD;
class MyDashStaminaBar;
class MyGameInstance;
class MySaveGame;

UCLASS()
class MULTIPLAYERTEST_API AMyPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* playerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCameraComponent* cameraComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* projectileSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AMyProjectile> projectileSpawnObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxPlayerMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float increasePlayerMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float decreasePlayerMoveSpeed;

	float currentPlayerMovementSpeedX{ 0 };
	float currentPlayerMovementSpeedY{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* cursorStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float cursorLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool showDrawShootHearDistance{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool showDrawStepHearDistance{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxCursorDistance{ 195 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxCameraDistance{ 2500.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USpringArmComponent* cameraAttach;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float cameraSpeed{ 300.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf< class UUserWidget> cursorWidgetTemplete;

	class AMyAIController* aiController;

	//has to be NULL for player to be able to shoot
	AMyProjectile* currentProjectile{ NULL };

	FVector2D viewportSize;

	FVector cameraFollowPosition;
	int numberBeforeMouseSetUp{ 0 };
	APlayerController* myPlayerController;

	bool madeSoundShoot{ false };
	bool madeSoundStep{ false };
	float madeSoundTimerShoot{ 0.0f };
	float madeSoundTimerStep{ 0.0f };

	FVector lastMouseCorrectPos;
	bool mouseSetUp{ false };
	FVector lastMouseDirection{ 0,0,0 };

	FVector lastMousePos{ 0,0,0 };
	FVector lastCameraCorrectPos{ 0, 0, 0 };
	bool shouldFreeView{ false };

	float currentDashTime{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxDashTime{ 1.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float dashSpeed{ 400.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxDelayBetweenDashes{ 5.0f };

	float currentDelayBetweenDashes;

	FVector dashDirection;

	class AMyHUD* myHUD;

	float moveDirectionX, moveDirectionY;

	bool hasWon{ false };

	bool hasBeenHit{ false };


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float parryDuration{ 0.2f };

	float currentParryTime{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* parryShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxDelayBetweenParry{ 3.0f };

	float currentDelayBetweenParry;


	void MoveHorizontal(float horizontal);

	void MoveVertical(float vertical);

	void LeftClickFunction();

	void RotatePlayerToMousePosition();

	void KeepPlayerInAir();

	void UpdateCursorMesh();

	void Die();

	void SetCameraFreeViewTrue();
	void SetCameraFreeViewFalse();

	void SetupPlayerController();

	void HandleSoundTimers(float DeltaTime);

	void PlayerDash();

	void PlayerParry();
};
