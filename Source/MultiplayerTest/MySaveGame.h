// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
//#include "MyAIController.h"
//#include "MyHUD.h"
//#include "MyCheckpoint.h"
#include "MySaveGame.generated.h"

/**
 *
 */
class MyHUD;
class MyCheckpoint;
class MyAIController;
class MyDashStaminaBar;
UCLASS()
class MULTIPLAYERTEST_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector playerPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString saveGameName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDateTime creationTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class AMyAIController*> aliveEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> aliveEnemiesNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< AActor*> enemyActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class AMyCheckpoint*> activatedCheckpoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< AActor*> aliveEnemyActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class AMyAIController*> enemyAI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> activatedCheckpointNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float seconds{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int minutes{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int hours{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int score{ 0 };


	class AMyHUD* myHUD;



	void SaveGame();

	void CreateSlot(const FString& name);



	UMySaveGame();
};
