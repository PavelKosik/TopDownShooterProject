// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "MyScoreManager.h"
#include "MyTimeManager.h"
#include "MyPlayer.h"
#include "MyPlayerWonMenu.h"
#include "MyDashStaminaBar.h"
#include "MyParryWidget.h"

AMyHUD::AMyHUD() {

	//gets reference to all the widgets
	static ConstructorHelpers::FClassFinder<UMyTimeManager> timeManagerWidgetAsset(TEXT("/Game/Blueprints/MyTimeManager_BP"));
	static ConstructorHelpers::FClassFinder<UMyScoreManager> scoreManagerWidgetAsset(TEXT("/Game/Blueprints/MyScoreManager_BP"));
	static ConstructorHelpers::FClassFinder<UMyDashStaminaBar> dashStaminaBarWidgetAsset(TEXT("/Game/Blueprints/MyDashStaminaBar_BP"));
	static ConstructorHelpers::FClassFinder<UMyPlayerWonMenu> myPlayerWonMenuWidgetAsset(TEXT("/Game/Blueprints/MyPlayerWonMenu_BP"));
	static ConstructorHelpers::FClassFinder<UUserWidget> youDiedWidgetAsset(TEXT("/Game/Blueprints/YouDiedWidget_BP"));
	static ConstructorHelpers::FClassFinder<UMyParryWidget> myParryWidgetAsset(TEXT("/Game/Blueprints/MyParryWidget_BP"));
	static ConstructorHelpers::FClassFinder<UUserWidget> myGameSavedWidgetAsset(TEXT("/Game/Blueprints/MyGameSavedWidget_BP"));

	//creates templetes which are used to create the widgets from
	timeManagerWidgetTemplete = timeManagerWidgetAsset.Class;
	scoreManagerWidgetTemplete = scoreManagerWidgetAsset.Class;
	dashStaminaBarWidgetTemplete = dashStaminaBarWidgetAsset.Class;
	myPlayerWonMenuWidgetTemplete = myPlayerWonMenuWidgetAsset.Class;
	youDiedWidgetTemplete = youDiedWidgetAsset.Class;
	myParryWidgetTemplete = myParryWidgetAsset.Class;
	myGameSavedWidgetTemplete = myGameSavedWidgetAsset.Class;
}

void AMyHUD::BeginPlay() {

	Super::BeginPlay();

	//adds timer widget to viewport so player can see his time
	timeManagerWidget = CreateWidget<UMyTimeManager>(GetWorld(), timeManagerWidgetTemplete);
	timeManagerWidget->AddToViewport();

	//adds score widget to viewport so player can see his score
	scoreManagerWidget = CreateWidget<UMyScoreManager>(GetWorld(), scoreManagerWidgetTemplete);
	scoreManagerWidget->AddToViewport();

	//adds dash stamina bar so that player can see if he can currently dash
	dashStaminaBar = CreateWidget<UMyDashStaminaBar>(GetWorld(), dashStaminaBarWidgetTemplete);
	dashStaminaBar->AddToViewport();

	myPlayer = dashStaminaBar->aMyPlayer;

	//adds the parry widget to viewport so that player can see if he can currently parry
	myParryWidget = CreateWidget<UMyParryWidget>(GetWorld(), myParryWidgetTemplete);
	myParryWidget->aMyPlayer = myPlayer;
	myParryWidget->AddToViewport();
}

void AMyHUD::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (myPlayer) {
		//makes sure that the player won menu is displayed only once
		if (myPlayer->hasWon && wonMenuDisplayed == false) {
			//creates the player won menu and adds it to viewport so that player can see it
			playerWonMenu = CreateWidget<UMyPlayerWonMenu>(GetWorld(), myPlayerWonMenuWidgetTemplete);
			playerWonMenu->myHUD = this;
			playerWonMenu->AddToViewport();
			//makes sure that the won menu won't be displayed again
			wonMenuDisplayed = true;
		}

		//if player has been killed by the enemies
		//then the you died widget is displayed to let him know that he was killed
		if (myPlayer->hasBeenHit) {
			youDiedWidget = CreateWidget<UUserWidget>(GetWorld(), youDiedWidgetTemplete);
			youDiedWidget->AddToViewport();
		}
	}

	else {
		myPlayer = dashStaminaBar->aMyPlayer;
		if (!myParryWidget->aMyPlayer) {
			myParryWidget->aMyPlayer = myPlayer;

		}
	}


	if (GetWorldTimerManager().IsTimerActive(gameSavedDisplayTimerHandle) == false) {
		//shows the widget that notifies player that the game was saved
		//the widget is shown for a timer which lasts 1.5 seconds
		if (shouldDisplaySaveWidget) {
			GetWorld()->GetTimerManager().SetTimer(gameSavedDisplayTimerHandle, this, &AMyHUD::HideGameSavedWidget, 1.5f, false);
			shouldDisplaySaveWidget = false;
			//if this is the first time the game was saved then the widget must be created
			//and the newly created widget is displayed
			if (gameSavedWidget == nullptr) {
				gameSavedWidget = CreateWidget<UUserWidget>(GetWorld(), myGameSavedWidgetTemplete);
				gameSavedWidget->AddToViewport();
			}
			//else the old reference to the created widget is used to display it
			else {
				gameSavedWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}

	}

}

void AMyHUD::HideGameSavedWidget() {
	//hides the widget
	if (gameSavedWidget!=nullptr) {
		gameSavedWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}