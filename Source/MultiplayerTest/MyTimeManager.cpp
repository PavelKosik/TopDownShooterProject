// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTimeManager.h"
#include "MyGameInstance.h"
#include "MySaveGame.h"

UMyTimeManager::UMyTimeManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}
void UMyTimeManager::NativeConstruct() {
	Super::NativeConstruct();
	myGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (myGameInstance != nullptr) {
		if (myGameInstance->mySaveGame != nullptr) {
			if (gameWon == false) {
				seconds = myGameInstance->mySaveGame->seconds;
				minutes = myGameInstance->mySaveGame->minutes;
				hours = myGameInstance->mySaveGame->hours;
			}
		}

	}
}

void UMyTimeManager::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (gameWon == false) {

		FString text = "Time: ";

		text.Append(FString::FromInt(hours));
		text.Append(":");
		text.Append(FString::FromInt(minutes));
		text.Append(":");
		text.Append(FString::FromInt(FGenericPlatformMath::RoundToInt(seconds)));

		TextBlock_69->SetText(FText::FromString(text));
		seconds += InDeltaTime;

		if (seconds >= 59) {
			minutes += 1;
			seconds = 0;
		}

		if (minutes >= 60) {
			hours += 1;
			minutes = 0;
		}


	}

	if (myGameInstance != nullptr) {
		if (myGameInstance->mySaveGame != nullptr) {
			if (gameWon == false) {
				myGameInstance->mySaveGame->seconds = seconds;
				myGameInstance->mySaveGame->minutes = minutes;
				myGameInstance->mySaveGame->hours = hours;
			}
		}

	}
	else {
		myGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}


}
