// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
//#include "MyGameInstance.h"
//#include "MyEnemyCharacter.h"
//#include "MyHUD.h"
//#include "MyAIController.h"
#include "MyCheckpoint.generated.h"

class MyPlayer;
class MyGameInstance;
class MyEnemyCharacter;
class MyHUD;
class MyAIController;
class MySaveGame;
class MyScoreManager;


UCLASS()
class MULTIPLAYERTEST_API AMyCheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCheckpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class UMyGameInstance* myGameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* checkpointBody;

	TArray< class AMyAIController*> enemyAI;
	TArray<AActor*> enemyActors;	
	TArray<AActor*> enemyAIActor;
	TArray<class AMyCheckpoint*> checkpoints;

	bool activated{ false };

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
