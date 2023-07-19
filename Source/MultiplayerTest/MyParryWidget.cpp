// Fill out your copyright notice in the Description page of Project Settings.


#include "MyParryWidget.h"


UMyParryWidget::UMyParryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	myPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
}


void UMyParryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {

	//UI lets player know if he can parry or how long he has to wait before he can parry again
	if (VerticalBox_0) {
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(VerticalBox_0->Slot);
		if (aMyPlayer) {		
			slot->SetSize(FVector2D((aMyPlayer->currentDelayBetweenParry * 30) / aMyPlayer->maxDelayBetweenParry, 30));
		}
	}
	
}