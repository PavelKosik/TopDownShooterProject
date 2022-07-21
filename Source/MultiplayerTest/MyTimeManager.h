// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Components/TextBlock.h"
//#include "MyGameInstance.h"
#include "MyTimeManager.generated.h"

/**
 * 
 */
class MyGameInstance;
class MySaveGame;

UCLASS()
class MULTIPLAYERTEST_API UMyTimeManager : public UUserWidget
{
	GENERATED_BODY()
public:
	UMyTimeManager(const FObjectInitializer& ObjectInitializer);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* TextBlock_69;

	bool gameWon{ false };
	float seconds{ 0 };
	int minutes{ 0 };
	int hours{ 0 };

	class UMyGameInstance* myGameInstance;
};
