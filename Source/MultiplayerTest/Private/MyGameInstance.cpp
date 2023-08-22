// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "MySaveGame.h"
#include "MyAIController.h"

bool UMyGameInstance::IsNewGame() {
	//checks if there is already a save slot
	//if there is returns false
	if (UGameplayStatics::DoesSaveGameExist(UNIQUE_SAVE_SLOT, 0)) {
		return false;
	}

	//else returns true
	return true;
}

bool UMyGameInstance::CreateNewSaveGame() {
	//if this is the first time player saves the game then new save game is created
	if (mySaveGame == nullptr) {
		USaveGame* newSaveGame = UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass());

		if (newSaveGame != nullptr) {
			mySaveGame = Cast<UMySaveGame>(newSaveGame);
		}

	}
	//else the slot created before is used
	else {
		mySaveGame->CreateSlot(UNIQUE_SAVE_SLOT);
	}
	//saves the game to the slot on the 0 position
	return UGameplayStatics::SaveGameToSlot(mySaveGame, UNIQUE_SAVE_SLOT, 0);
}

bool UMyGameInstance::LoadGame() {
	mySaveGame = nullptr;

	//loads the game from the current slot
	USaveGame* slot = UGameplayStatics::LoadGameFromSlot(UNIQUE_SAVE_SLOT, 0);

	//lets the function caller know that the game loaded successfully
	if (slot != nullptr) {
		mySaveGame = Cast<UMySaveGame>(slot);

		if (mySaveGame != nullptr) {
			return true;
		}
	}

	//if no slot was find that the game couldn't be loaded
	return false;
}

bool UMyGameInstance::SaveGame(FVector playerPos, TArray<FString> aliveEnemiesNames, TArray<FString> activatedCheckpointNames, int score) {
	//if this is the first time saving the game then the save game has to be created
	if (mySaveGame == nullptr) {
		USaveGame* newSaveGame = UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass());

		//writes all the values from this game instance to the save
		if (newSaveGame != nullptr) {
			mySaveGame = Cast<UMySaveGame>(newSaveGame);
			mySaveGame->playerPos = playerPos;
			mySaveGame->aliveEnemiesNames = aliveEnemiesNames;
			mySaveGame->activatedCheckpointNames = activatedCheckpointNames;
			mySaveGame->score = score;

		}

	}
	//writes all the values from this game instance to the save
	else {
		mySaveGame->playerPos = playerPos;
		mySaveGame->aliveEnemiesNames = aliveEnemiesNames;
		mySaveGame->activatedCheckpointNames = activatedCheckpointNames;
		mySaveGame->score = score;

	}
	//saves to slot
	return UGameplayStatics::SaveGameToSlot(mySaveGame, UNIQUE_SAVE_SLOT, 0);


	return false;
}