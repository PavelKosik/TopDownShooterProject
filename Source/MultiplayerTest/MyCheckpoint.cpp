// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCheckpoint.h"
#include "MyPlayer.h"
#include "MyGameInstance.h"
#include "MyEnemyCharacter.h"
#include "MyHUD.h"
#include "MyAIController.h"
#include "MySaveGame.h"
#include "MyScoreManager.h"

// Sets default values
AMyCheckpoint::AMyCheckpoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	checkpointBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Checkpoint body"));
}

// Called when the game starts or when spawned
void AMyCheckpoint::BeginPlay()
{
	Super::BeginPlay();
	checkpointBody->OnComponentBeginOverlap.AddDynamic(this, &AMyCheckpoint::OnOverlapBegin);
	myGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (myGameInstance != nullptr) {

		if (myGameInstance->mySaveGame != nullptr) {

			for (int i = 0; i < myGameInstance->mySaveGame->activatedCheckpointNames.Num(); i++) {
				//prevents player from activating the same checkpoint twice on level reload
				if (myGameInstance->mySaveGame->activatedCheckpointNames[i] == (GetFName()).ToString()) {
					activated = true;
					return;
				}
			}
		}
	}
}

// Called every frame
void AMyCheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (myGameInstance == nullptr) {
		myGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}

}

void AMyCheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	bool enemySearching = false;
	enemyActors.Empty();
	enemyAI.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyAIController::StaticClass(), enemyActors);
	for (int i = 0; i < enemyActors.Num(); i++) {
		enemyAI.Add(Cast<AMyAIController>(enemyActors[i]));
		//if any of these booleans is true then enemy must be currently looking for player
		//in that were the case player will be unable to save when he passes the checkpoint
		if (enemyAI[i]->goingToLastSawPosition || enemyAI[i]->lookingForPlayer || enemyAI[i]->sawBullet || enemyAI[i]->myEnemyLogic->foundPlayer) {
			enemySearching = true;
		}
	}

	//checkpoints save the game only if enemies aren't looking for player
	if (!enemySearching) {
		//each checkpoint can be used only once
		if (!activated) {
			//checkpoints can be only used by the player model
			if (OtherComp->ComponentHasTag("PlayerMesh")) {

				if (myGameInstance != nullptr) {
					activated = true;

					AMyHUD* myHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
					if (myHUD) {
						//this shows a widget which lets player know that the game was saved
						myHUD->shouldDisplaySaveWidget = true;
					}

					//variables in the current level that need to be saved on the checkpoint activation
					TArray<FString> aliveEnemiesNames;
					TArray<FString> activatedCheckpointNames;
					enemyActors.Empty();
					enemyAI.Empty();
					checkpoints.Empty();
					TArray<AActor*> checkpointsActors;

					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyCheckpoint::StaticClass(), checkpointsActors);

					//saves all the activated checkpoints
					for (int i = 0; i < checkpointsActors.Num(); i++) {
						AMyCheckpoint* thisCheckpoint = Cast<AMyCheckpoint>(checkpointsActors[i]);
						if (thisCheckpoint->activated) {
							activatedCheckpointNames.Add(checkpointsActors[i]->GetName());
						}
					}

					//gets all the enemies which will be used to find all the alive enemies
					UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Enemy", enemyActors);
					for (int i = 0; i < enemyActors.Num(); i++) {

						enemyAI.Add(Cast<AMyAIController>(enemyActors[i]->GetInstigatorController()));

					}

					//if the enemy is still alive when player enters the checkpoint then his name is stored
					//this array is used to decide which enemies can be spawned on level reload
					for (int i = 0; i < enemyAI.Num(); i++) {
						if (enemyAI[i]) {
							if (enemyAI[i]->killed) {
								continue;
							}

							else {
								aliveEnemiesNames.Add(*enemyActors[i]->GetName());
							}
						}

					}


					if (myGameInstance->mySaveGame != nullptr) {
						//saves the player score
						if (myHUD) {
							myGameInstance->mySaveGame->score = myHUD->scoreManagerWidget->score;
						}
						//if there was a save already then it's overwritten
						myGameInstance->mySaveGame->playerPos = OtherActor->GetActorLocation();
						myGameInstance->mySaveGame->aliveEnemiesNames = aliveEnemiesNames;
						myGameInstance->mySaveGame->activatedCheckpointNames = activatedCheckpointNames;
					}
					else {
						//if there is no save then it's created
						bool createdNewSave = myGameInstance->CreateNewSaveGame();
					}
					//saves the game with current values
					if (myGameInstance->SaveGame(OtherActor->GetActorLocation(), aliveEnemiesNames, activatedCheckpointNames, Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn())->myHUD->scoreManagerWidget->score)) {
						return;
					}
				}

			}
		}
	}
}



