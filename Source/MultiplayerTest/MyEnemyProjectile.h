// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "MyPlayer.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyEnemyProjectile.generated.h"

class MyPlayer;
class MyEnemyCharacter;
class MyAIController;
UCLASS()
class MULTIPLAYERTEST_API AMyEnemyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyEnemyProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* projectileBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UProjectileMovementComponent* projectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float projectileMoveSpeed{300};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AMyPlayer* playerObject;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float travelledDistanceMax{ 10000 };


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float deathTimeDuration{ 1 };

	float travelledDistance{ 0 };

	bool bounced{ false };

	bool travelDistanceResetedAfterParry{ false };

	FVector positionToGoTo;
	FVector startPosition;

	FTimerHandle timerHandle;

	bool collidedWithPlayer{ false };

	bool parried{ false };

	UFUNCTION()
		void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void StartKillPlayerTimer();

	UFUNCTION()
		void KillPlayer();
};
