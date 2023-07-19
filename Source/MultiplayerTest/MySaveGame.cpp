// Fill out your copyright notice in the Description page of Project Settings.


#include "MySaveGame.h"
#include "MyAIController.h"
#include "MyHUD.h"
#include "MyCheckpoint.h"
#include "MyDashStaminaBar.h"
UMySaveGame::UMySaveGame()
{
	
}

void UMySaveGame::SaveGame() {
	
}
//Creates a save slot
void UMySaveGame::CreateSlot(const FString& name) {
	saveGameName = name;
	creationTime = FDateTime::Now();
	myHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());

	playerPos = myHUD->dashStaminaBar->aMyPlayer->GetActorLocation();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyAIController::StaticClass(), enemyActors);
}