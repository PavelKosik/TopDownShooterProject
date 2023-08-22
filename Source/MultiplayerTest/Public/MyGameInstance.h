// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
//#include "MySaveGame.h"
//#include "MyAIController.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
class MyAIController;
class MySaveGame;
UCLASS()
class MULTIPLAYERTEST_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMySaveGame* mySaveGame;

	FString UNIQUE_SAVE_SLOT="SaveData_0";

	bool IsNewGame();

	bool CreateNewSaveGame();

	bool LoadGame();

	bool SaveGame(FVector playerPos, TArray<class AMyAIController*> aliveEnemies, TArray< AActor*> enemyActors, TArray< AActor*> aliveEnemyActors, TArray<class AMyCheckpoint*>, TArray<class AMyAIController*> enemyAI, TArray<FString> names, TArray<FString> activatedCheckpointNames);
	bool SaveGame(FVector playerPos, TArray<FString> aliveEnemiesNames, TArray<FString> activatedCheckpointNames, int score);

};
