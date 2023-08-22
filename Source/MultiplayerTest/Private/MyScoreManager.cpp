// Fill out your copyright notice in the Description page of Project Settings.


#include "MyScoreManager.h"
#include "MyAIController.h"
#include "MyGameInstance.h"
#include "MyEnemyCharacter.h"
#include "MySaveGame.h"

UMyScoreManager::UMyScoreManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyAIController::StaticClass(), enemyActors);
	//gets reference to all the enemies to use for scores 
	for (int i = 0; i < enemyActors.Num(); i++) {
		enemyAI.Add(Cast<AMyAIController>(enemyActors[i]));
		enemyKilled.Add(false);
	}	
}

void UMyScoreManager::NativeConstruct() {
	Super::NativeConstruct();
	//tries to load the score from save
	myGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (myGameInstance != nullptr) {
		if (myGameInstance->mySaveGame != nullptr) {
				score = myGameInstance->mySaveGame->score;			
		}

	}
}

void UMyScoreManager::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	FString text = "Score: ";
	text.Append(FString::FromInt(score));
	TextBlock_36->SetText(FText::FromString(text));
	//the score increase is only displayed for a certain period of time
	displayTime -= InDeltaTime;
	if (displayTime<=0.0f) {
		TextBlock_74->SetText(FText::FromString(""));
	}
	//player is rewarded for killing enemies by increasing his score
	for (int i = 0; i < enemyActors.Num(); i++) {		
		if (enemyAI[i]->killed &&enemyKilled[i]==false) {
			displayTime = 1.0f;
			//player gets more score points if the enemy didn't see him before he was killed by the player
			if (enemyAI[i]->myEnemyLogic->playerFoundThisFrame==false) {
				score += 150;
				TextBlock_74->SetText(FText::FromString("+ Score 150"));

				enemyKilled[i] = true;
			}
			//if enemy saw player before he was killed then the score for the kill 
			//depends on how many projectiles the enemy was able to fire
			else if(enemyAI[i]->numberOfShotProjectiles<=1){
				score += 100;
				

				TextBlock_74->SetText(FText::FromString("+ Score 100"));

				enemyKilled[i] = true;
			}

			else if(enemyAI[i]->numberOfShotProjectiles < 5){
				score += 100-20* enemyAI[i]->numberOfShotProjectiles;
				FString scoreAddText = "+ Score: ";
				scoreAddText.Append(FString::SanitizeFloat(100 - 20 * enemyAI[i]->numberOfShotProjectiles));
				TextBlock_74->SetText(FText::FromString(scoreAddText));
				enemyKilled[i] = true;
			}

			else if (enemyAI[i]->numberOfShotProjectiles <= 6) {
				score += 20;
				TextBlock_74->SetText(FText::FromString("+ Score 20"));
				enemyKilled[i] = true;
			}

			else {
				score += 10;
				TextBlock_74->SetText(FText::FromString("+ Score 10"));
				enemyKilled[i] = true;
			}
		}
	}
}
