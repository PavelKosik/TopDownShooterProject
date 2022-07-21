// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerController.h"
#include "Components/Image.h"
#include "MyParryWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERTEST_API UMyParryWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UMyParryWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UImage* Image_0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UVerticalBox* VerticalBox_0;

	APlayerController* myPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class  AMyPlayer* aMyPlayer;
};
