// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
//#include "MyEnemyCharacter.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
//#include "MyPlayer.h"
#include "Kismet/KismetMathLibrary.h"
//#include "MyEnemyProjectile.h"
#include "Engine/TargetPoint.h"
#include "MyAIController.generated.h"

/**
 * 
 */
class MyPlayer;
class MyEnemyProjectile;
class MyEnemyCharacter;
UCLASS()
class MULTIPLAYERTEST_API AMyAIController : public AAIController
{
	GENERATED_BODY()
	


public:   
        APawn* myPawn;

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
            class AMyEnemyCharacter* myEnemyLogic;

        
        UPROPERTY(EditAnywhere, BlueprintReadWrite)
            TSubclassOf<class AMyEnemyProjectile> projectileSpawnObject;

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
            float timeBetweenShotsMax{ 2.0f };

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
            float distanceToStartShootingAt{300};


        UPROPERTY(EditAnywhere, BlueprintReadWrite)
            float maxHearDistanceShoot{ 2000 };

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
            float maxHearDistanceStep{ 500 };

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
            int currentPathIndex{ 0 };

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
            bool finishedPath{ false };

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
            float maxWaitTime{ 0.5f };
        
        UPROPERTY(EditAnywhere, BlueprintReadWrite)
            float searchRotationSpeed{ 100.0f };
        
        bool setStart{ true };

        float currentTimeBetweenShots{0.0f};
        bool killed{ false };
        int numberOfShotProjectiles{ 0 };
        UNavigationSystemV1* navMesh;
        bool goingToLastSawPosition{ false };
        class AMyPlayer* player;       
        FVector playerHeardPosition;
        bool shooting{ false };
        bool lookingForPlayer{ false };
        bool waiting{ false };
        bool startMove{ false };       
        FTimerHandle timerHandle;
        FQuat rotateAroundRotation;
        bool startedRotation{ false };
        float howMuchEnemyAlreadyRotated{ 0.0f };

        bool sawBullet{ false };
        FVector lastBulletPos;
        bool goingToBulletLastSawPos{ false };
        FVector dirToContinueWalking;
        FRotator staticEnemyRotation;

        float stationaryRotationSpeed{ 3.0f };

        float deltaTime;

        bool returningToStationaryPoint{ false };

        void BeginPlay() override;
        void Tick(float DeltaTime) override;
        void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
        //void MoveToRandomPoint();
        void MoveToPlayer();
        void ShootPlayer();
        void WalkToLastSeenPosition();
        void LookAtPlayer();
        void HandleHearing();  
        void RotateAroundSearch();     
        void FollowPath();
        void Wait();
        void FindPointNearLastSawBulletPos();
        void RotateStationaryEnemy();
};
