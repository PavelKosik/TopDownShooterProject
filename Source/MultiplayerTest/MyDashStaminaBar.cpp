// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDashStaminaBar.h"

UMyDashStaminaBar::UMyDashStaminaBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	myPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void UMyDashStaminaBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
		
	if (VerticalBox_147) {
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(VerticalBox_147->Slot);
		if (aMyPlayer) {
			slot->SetSize(FVector2D((aMyPlayer->currentDelayBetweenDashes * 30)/ aMyPlayer->maxDelayBetweenDashes, 30));
		}
	}
}
