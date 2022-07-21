// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerWonMenu.h"
#include "MyHUD.h"
#include "MyScoreManager.h"
#include "MyTimeManager.h"

UMyPlayerWonMenu::UMyPlayerWonMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMyPlayerWonMenu::NativeConstruct() {
	Super::NativeConstruct();
	NextButton->OnClicked.AddDynamic(this, &UMyPlayerWonMenu::NextButtonOnClick);
	RetryButton->OnClicked.AddDynamic(this, &UMyPlayerWonMenu::RetryButtonOnClick);
	ExitButton->OnClicked.AddDynamic(this, &UMyPlayerWonMenu::ExitButtonOnClick);

}

void UMyPlayerWonMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	FString scoreText = "Your Score: ";
	FString timeText = "Your Time: ";
	if (myHUD) {
		scoreText.Append(FString::FromInt(myHUD->scoreManagerWidget->score));
		YourScoreTextBlock->SetText(FText::FromString(scoreText));

		timeText.Append(FString::FromInt(myHUD->timeManagerWidget->hours));
		timeText.Append(":");
		timeText.Append(FString::FromInt(myHUD->timeManagerWidget->minutes));
		timeText.Append(":");
		timeText.Append(FString::FromInt(FGenericPlatformMath::RoundToInt(myHUD->timeManagerWidget->seconds)));

		YourTimeTextBlock->SetText(FText::FromString(timeText));

		APlayerController* myPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (myPlayer != NULL)
		{
			myHUD->timeManagerWidget->gameWon = true;
			myPlayer->SetPause(true);
		}

		else {
			UE_LOG(LogTemp, Warning, TEXT("PlAYER NOT FOUND"));
		}
	}
}

void UMyPlayerWonMenu::NextButtonOnClick() {
	FName currentLevel = GetWorld()->GetFName();
	FName nextLevel="";
	for (int i = 0; i < levelNames.Num(); i++) {
		if (levelNames[i]==currentLevel) {
			if (levelNames.IsValidIndex(i+1)) {
				nextLevel = levelNames[i + 1];
			}

			else {
				UE_LOG(LogTemp, Warning, TEXT("NOT A VALID NEXT LEVEL1"));
			}
		}
	}

	if (nextLevel!="") {
		UGameplayStatics::OpenLevel(this, nextLevel, false);
	}

	else {
		UE_LOG(LogTemp, Warning, TEXT("NOT A VALID NEXT LEVEL2"));
	}

}

void UMyPlayerWonMenu::RetryButtonOnClick() {
	UE_LOG(LogTemp, Warning, TEXT("%s"), *(GetWorld()->GetFName()).ToString());

	UGameplayStatics::OpenLevel(this, FName(GetWorld()->GetFName()), false);
}

void UMyPlayerWonMenu::ExitButtonOnClick() {
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, true);
}