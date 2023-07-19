// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
//#include "MyScoreManager.h"
//#include "MyTimeManager.h"
//#include "MyPlayer.h"
//#include "MyPlayerWonMenu.h"
//#include "MyDashStaminaBar.h"
//#include "MyParryWidget.h"
#include "MyHUD.generated.h"

/**
 * 
 */
class MyScoreManager;
class MyTimeManager;
class MyPlayer;
class MyPlayerWonMenu;
class MyDashStaminaBar;
class MyParryWidget;
UCLASS()
class MULTIPLAYERTEST_API AMyHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMyHUD();


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	TSubclassOf<class UUserWidget> timeManagerWidgetTemplete;
	TSubclassOf<class UUserWidget> scoreManagerWidgetTemplete;
	TSubclassOf<class UUserWidget> dashStaminaBarWidgetTemplete;
	TSubclassOf<class UUserWidget> myPlayerWonMenuWidgetTemplete;
	TSubclassOf<class UUserWidget> youDiedWidgetTemplete;
	TSubclassOf<class UUserWidget> myParryWidgetTemplete;
	TSubclassOf<class UUserWidget> myGameSavedWidgetTemplete;


	class UMyTimeManager* timeManagerWidget;
	class UMyScoreManager* scoreManagerWidget;
	class UMyDashStaminaBar* dashStaminaBar;
	class UMyPlayerWonMenu* playerWonMenu;
	class AMyPlayer* myPlayer;
	class UUserWidget* youDiedWidget;
	class UMyParryWidget* myParryWidget;
	class UUserWidget* gameSavedWidget;

	bool wonMenuDisplayed{ false };
	bool shouldDisplaySaveWidget{ false };

	FTimerHandle gameSavedDisplayTimerHandle;

	void HideGameSavedWidget();
};
