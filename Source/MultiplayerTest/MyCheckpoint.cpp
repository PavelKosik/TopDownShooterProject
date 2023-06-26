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
		if (enemyAI[i]->goingToLastSawPosition || enemyAI[i]->lookingForPlayer || enemyAI[i]->sawBullet || enemyAI[i]->myEnemyLogic->foundPlayer) {
			enemySearching = true;
		}
	}


	if (!enemySearching) {
		if (!activated) {
			if (OtherComp->ComponentHasTag("PlayerMesh")) {

				if (myGameInstance != nullptr) {
					activated = true;

					AMyHUD* myHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
					if (myHUD) {
						myHUD->shouldDisplaySaveWidget = true;
					}



					TArray<FString> aliveEnemiesNames;
					TArray<FString> activatedCheckpointNames;
					enemyActors.Empty();
					enemyAI.Empty();
					checkpoints.Empty();
					TArray<AActor*> checkpointsActors;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyCheckpoint::StaticClass(), checkpointsActors);


					for (int i = 0; i < checkpointsActors.Num(); i++) {
						AMyCheckpoint* thisCheckpoint = Cast<AMyCheckpoint>(checkpointsActors[i]);
						if (thisCheckpoint->activated) {
							activatedCheckpointNames.Add(checkpointsActors[i]->GetName());
						}
					}


					UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Enemy", enemyActors);
					for (int i = 0; i < enemyActors.Num(); i++) {

						enemyAI.Add(Cast<AMyAIController>(enemyActors[i]->GetInstigatorController()));

					}

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

						if (myHUD) {
							myGameInstance->mySaveGame->score = myHUD->scoreManagerWidget->score;
						}

						myGameInstance->mySaveGame->playerPos = OtherActor->GetActorLocation();
						myGameInstance->mySaveGame->aliveEnemiesNames = aliveEnemiesNames;
						myGameInstance->mySaveGame->activatedCheckpointNames = activatedCheckpointNames;
					}
					else {

						bool createdNewSave = myGameInstance->CreateNewSaveGame();
					}

					if (myGameInstance->SaveGame(OtherActor->GetActorLocation(), aliveEnemiesNames, activatedCheckpointNames, Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn())->myHUD->scoreManagerWidget->score)) {
						return;
					}
				}

			}
		}
	}
}



