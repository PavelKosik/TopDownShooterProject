// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "MyHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MyPlayerWonMenu.generated.h"

/**
 *
 */
class MyHUD;
class MyScoreManager;
class MyTimeManager;

UCLASS()
class MULTIPLAYERTEST_API UMyPlayerWonMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UMyPlayerWonMenu(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* NextButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* RetryButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* ExitButton;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* YourScoreTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* YourTimeTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FName> levelNames;

	class AMyHUD* myHUD;

	UFUNCTION()
		void NextButtonOnClick();

	UFUNCTION()
		void RetryButtonOnClick();

	UFUNCTION()
		void ExitButtonOnClick();
};
