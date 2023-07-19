// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
//#include "MyAIController.h"
#include "Engine/StaticMesh.h"
//#include "MyPlayer.h"
#include "Materials/MaterialInstance.h"
#include "Kismet/KismetMathLibrary.h"
//#include "MyGameInstance.h"
#include "GameFramework/Character.h"
#include "MyEnemyCharacter.generated.h"

class MyAIController;
class MyPlayer;
class MyGameInstance;
class MySaveGame;
UCLASS()
class MULTIPLAYERTEST_API AMyEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyEnemyCharacter();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UProceduralMeshComponent* proceduralMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInstance* materialMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInstance* materialMesh2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* projectileSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int widthEnemyCanSee{ 15 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float distanceBetweenVisionLines{ 3 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float distanceEnemyCanSee{ 300 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UArrowComponent* arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* enemyBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool checkedLastSeenPositon{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool playerFoundThisFrame{ false };

	class AMyPlayer* player;

	TArray<FVector> myVertices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool foundPlayer{ false };

	FVector lastPlayerSawPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AMyAIController* myCont;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UStaticMeshComponent> waypointPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<UStaticMeshComponent>> pathWaypoint;
		*/

		//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int numberOfWaypoints{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh* staticMeshCube;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints", META = (DisplayName = "StaticMeshPlacer"))
		UStaticMeshComponent* staticMeshPlacer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints", META = (Name = "Spawn Waypoint", DisplayAfter = "StaticMeshPlacer"))
		bool shouldSpawnWaypoint{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		TArray<FVector> waypointPositons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		bool displayWaypoints{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FColor neutralColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FColor searchColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FColor attackColor;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void PostInitializeComponents() override;

	void HandleRespawn();
};
