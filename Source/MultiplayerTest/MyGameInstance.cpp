// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "MySaveGame.h"
#include "MyAIController.h"

bool UMyGameInstance::IsNewGame() {
	if (UGameplayStatics::DoesSaveGameExist(UNIQUE_SAVE_SLOT, 0)) {
		return false;
	}

	return true;
}

bool UMyGameInstance::CreateNewSaveGame() {
	if (mySaveGame == nullptr) {
		USaveGame* newSaveGame = UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass());

		if (newSaveGame != nullptr) {
			mySaveGame = Cast<UMySaveGame>(newSaveGame);
		}

	}

	else {
		mySaveGame->CreateSlot(UNIQUE_SAVE_SLOT);
	}

	return UGameplayStatics::SaveGameToSlot(mySaveGame, UNIQUE_SAVE_SLOT, 0);
}

bool UMyGameInstance::LoadGame() {
	mySaveGame = nullptr;

	USaveGame* slot = UGameplayStatics::LoadGameFromSlot(UNIQUE_SAVE_SLOT, 0);

	if (slot != nullptr) {
		mySaveGame = Cast<UMySaveGame>(slot);

		if (mySaveGame != nullptr) {
			return true;
		}
	}

	return false;
}

bool UMyGameInstance::SaveGame(FVector playerPos, TArray<class AMyAIController*> aliveEnemies, TArray< AActor*> enemyActors, TArray< AActor*> aliveEnemyActors, TArray<class AMyCheckpoint*>, TArray<class AMyAIController*> enemyAI, TArray<FString> aliveEnemiesNames, TArray<FString> activatedCheckpointNames) {
	if (mySaveGame == nullptr) {
		USaveGame* newSaveGame = UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass());

		if (newSaveGame != nullptr) {
			mySaveGame = Cast<UMySaveGame>(newSaveGame);
			mySaveGame->playerPos = playerPos;
			mySaveGame->aliveEnemies = aliveEnemies;
			mySaveGame->enemyActors = enemyActors;
			mySaveGame->aliveEnemyActors = aliveEnemyActors;
			mySaveGame->enemyAI = enemyAI;
			mySaveGame->aliveEnemiesNames = aliveEnemiesNames;
			mySaveGame->activatedCheckpointNames = activatedCheckpointNames;
		}

	}
	else {
		mySaveGame->playerPos = playerPos;
		mySaveGame->aliveEnemies = aliveEnemies;
		mySaveGame->enemyActors = enemyActors;
		mySaveGame->aliveEnemyActors = aliveEnemyActors;
		mySaveGame->enemyAI = enemyAI;
		mySaveGame->aliveEnemiesNames = aliveEnemiesNames;
		mySaveGame->activatedCheckpointNames = activatedCheckpointNames;

	}

	return UGameplayStatics::SaveGameToSlot(mySaveGame, UNIQUE_SAVE_SLOT, 0);


	return false;
}

bool UMyGameInstance::SaveGame(FVector playerPos, TArray<FString> aliveEnemiesNames, TArray<FString> activatedCheckpointNames, int score) {
	if (mySaveGame == nullptr) {
		USaveGame* newSaveGame = UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass());

		if (newSaveGame != nullptr) {
			mySaveGame = Cast<UMySaveGame>(newSaveGame);
			mySaveGame->playerPos = playerPos;
			mySaveGame->aliveEnemiesNames = aliveEnemiesNames;
			mySaveGame->activatedCheckpointNames = activatedCheckpointNames;
			mySaveGame->score = score;

		}

	}
	else {
		mySaveGame->playerPos = playerPos;
		mySaveGame->aliveEnemiesNames = aliveEnemiesNames;
		mySaveGame->activatedCheckpointNames = activatedCheckpointNames;
		mySaveGame->score = score;

	}

	return UGameplayStatics::SaveGameToSlot(mySaveGame, UNIQUE_SAVE_SLOT, 0);


	return false;
}