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
#include "MyDashStaminaBar.generated.h"

/**
 *
 */
UCLASS()
class MULTIPLAYERTEST_API UMyDashStaminaBar : public UUserWidget
{
	GENERATED_BODY()

public:

	UMyDashStaminaBar(const FObjectInitializer& ObjectInitializer);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UImage* Image_0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UVerticalBox* VerticalBox_147;

	 APlayerController* myPlayer;

	 UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class  AMyPlayer* aMyPlayer;
};
