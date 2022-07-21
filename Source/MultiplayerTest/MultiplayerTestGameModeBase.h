// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "MyHUD.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
//#include "MyGameInstance.h"
#include "MultiplayerTestGameModeBase.generated.h"

/**
 * 
 */
class MyHUD;
class MyGameInstance;
UCLASS()
class MULTIPLAYERTEST_API AMultiplayerTestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;

public:

	class UMyGameInstance* gameInstance;

	AMultiplayerTestGameModeBase();

	void OnNewGame(FName mapToLoad);

	void OnLoadGame(FName maptoLoad);

};
