// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
//#include "MyAIController.h"
//#include "MyGameInstance.h"
#include "Components/TextBlock.h"
#include "MyScoreManager.generated.h"

/**
 *
 */
class MyAIController;
class MyGameInstance;
class MyEnemyCharacter;
class MySaveGame;

UCLASS()
class MULTIPLAYERTEST_API UMyScoreManager : public UUserWidget
{
	GENERATED_BODY()

public:
	UMyScoreManager(const FObjectInitializer& ObjectInitializer);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* TextBlock_36;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* TextBlock_74;

	TArray<AActor*> enemyActors;
	TArray< class AMyAIController*> enemyAI;
	TArray<bool> enemyKilled;

	float displayTime{ 0 };

	int score{ 0 };

	class UMyGameInstance* myGameInstance;

};
