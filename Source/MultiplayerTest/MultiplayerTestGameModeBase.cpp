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

	if (gameInstance!= nullptr) {
		bool isNewGame = gameInstance->IsNewGame();
	}
}

void AMultiplayerTestGameModeBase::OnNewGame(FName mapToLoad) {
	if (gameInstance == nullptr) return;

	if (gameInstance->CreateNewSaveGame()) {
		UGameplayStatics::OpenLevel(GetWorld(), mapToLoad, true);
	}
}

void AMultiplayerTestGameModeBase::OnLoadGame(FName maptoLoad) {
	UGameplayStatics::OpenLevel(GetWorld(), maptoLoad, true);
}