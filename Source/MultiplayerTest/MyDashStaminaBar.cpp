// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDashStaminaBar.h"

UMyDashStaminaBar::UMyDashStaminaBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	myPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0);
//	aMyPlayer = Cast<AMyPlayer>(myPlayer);
}

void UMyDashStaminaBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
		
	if (VerticalBox_147) {
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(VerticalBox_147->Slot);
		if (aMyPlayer) {
			//myPlayer->currentDashTime *
			slot->SetSize(FVector2D((aMyPlayer->currentDelayBetweenDashes * 30)/ aMyPlayer->maxDelayBetweenDashes, 30));
		}

		else {
			//myPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		//	aMyPlayer = Cast<AMyPlayer>(myPlayer);
			UE_LOG(LogTemp, Warning, TEXT("PLAYER DOESNT WORK"));

		}
		/*FVector2D size = slot->GetSize();
		UE_LOG(LogTemp, Warning, TEXT("%f,%f"), size.X, size.Y);*/



	}
	/*if (slot) {
		UE_LOG(LogTemp, Warning, TEXT("%f,%f"), size.X, size.Y);
	}*/
}
