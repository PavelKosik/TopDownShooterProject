// Copyright Epic Games, Inc. All Rights Reserved.


#include "MultiplayerTestGameModeBase.h"
#include "MyGameInstance.h"
#include "MyHUD.h"

AMultiplayerTestGameModeBase::AMultiplayerTestGameModeBase() : Super() {
	HUDClass = AMyHUD::StaticClass();
}

void AMultiplayerTestGameModeBase::BeginPlay() {
	Super::BeginPlay();

	gameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (gameInstance != nullptr) {
		bool isNewGame = gameInstance->IsNewGame();
	}
}

void AMultiplayerTestGameModeBase::OnNewGame(FName mapToLoad) {
	if (gameInstance == nullptr)
	{
		return;
	}
	//opens the game level
	if (gameInstance->CreateNewSaveGame()) {
		UGameplayStatics::OpenLevel(GetWorld(), mapToLoad, true);
	}
}

void AMultiplayerTestGameModeBase::OnLoadGame(FName maptoLoad) {
	//loads the game level
	UGameplayStatics::OpenLevel(GetWorld(), maptoLoad, true);
}