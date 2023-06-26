// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "MyScoreManager.h"
#include "MyTimeManager.h"
#include "MyPlayer.h"
#include "MyPlayerWonMenu.h"
#include "MyDashStaminaBar.h"
#include "MyParryWidget.h"

AMyHUD::AMyHUD() {

	static ConstructorHelpers::FClassFinder<UMyTimeManager> timeManagerWidgetAsset(TEXT("/Game/Blueprints/MyTimeManager_BP"));
	static ConstructorHelpers::FClassFinder<UMyScoreManager> scoreManagerWidgetAsset(TEXT("/Game/Blueprints/MyScoreManager_BP"));
	static ConstructorHelpers::FClassFinder<UMyDashStaminaBar> dashStaminaBarWidgetAsset(TEXT("/Game/Blueprints/MyDashStaminaBar_BP"));
	static ConstructorHelpers::FClassFinder<UMyPlayerWonMenu> myPlayerWonMenuWidgetAsset(TEXT("/Game/Blueprints/MyPlayerWonMenu_BP"));
	static ConstructorHelpers::FClassFinder<UUserWidget> youDiedWidgetAsset(TEXT("/Game/Blueprints/YouDiedWidget_BP"));
	static ConstructorHelpers::FClassFinder<UMyParryWidget> myParryWidgetAsset(TEXT("/Game/Blueprints/MyParryWidget_BP"));
	static ConstructorHelpers::FClassFinder<UUserWidget> myGameSavedWidgetAsset(TEXT("/Game/Blueprints/MyGameSavedWidget_BP"));


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

	timeManagerWidget = CreateWidget<UMyTimeManager>(GetWorld(), timeManagerWidgetTemplete);
	timeManagerWidget->AddToViewport();

	scoreManagerWidget = CreateWidget<UMyScoreManager>(GetWorld(), scoreManagerWidgetTemplete);
	scoreManagerWidget->AddToViewport();


	dashStaminaBar = CreateWidget<UMyDashStaminaBar>(GetWorld(), dashStaminaBarWidgetTemplete);
	dashStaminaBar->AddToViewport();

	myPlayer = dashStaminaBar->aMyPlayer;

	myParryWidget = CreateWidget<UMyParryWidget>(GetWorld(), myParryWidgetTemplete);
	myParryWidget->aMyPlayer = myPlayer;
	myParryWidget->AddToViewport();
}

void AMyHUD::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (myPlayer) {
		if (myPlayer->hasWon && wonMenuDisplayed == false) {
			playerWonMenu = CreateWidget<UMyPlayerWonMenu>(GetWorld(), myPlayerWonMenuWidgetTemplete);
			playerWonMenu->myHUD = this;
			playerWonMenu->AddToViewport();
			wonMenuDisplayed = true;
		}

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

	if (GetWorldTimerManager().IsTimerActive(timerHandle) == false) {

		if (shouldDisplaySaveWidget) {
			GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AMyHUD::HideGameSavedWidget, 1.5f, false);
			shouldDisplaySaveWidget = false;
			if (gameSavedWidget == nullptr) {
				gameSavedWidget = CreateWidget<UUserWidget>(GetWorld(), myGameSavedWidgetTemplete);
				gameSavedWidget->AddToViewport();
			}

			else {
				gameSavedWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}

	}

}

void AMyHUD::HideGameSavedWidget() {
	if (gameSavedWidget!=nullptr) {
		gameSavedWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}