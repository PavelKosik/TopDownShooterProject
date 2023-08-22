// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "MyPlayer.h"
#include "Materials/MaterialInstance.h"
//#include "MyAIController.h"
//#include "MyEnemyCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyProjectile.generated.h"

class MyPlayer;
class MyAIController;
class MyEnemyCharacter;
UCLASS()
class MULTIPLAYERTEST_API AMyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		UStaticMeshComponent* projectileMesh;

	FVector positionToGoTo;
	FVector startPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int maximalNumberOfBounces=5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float projectileMoveSpeed;

	int currentNumberOfBounces{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AMyPlayer* playerObject;

	bool bounced{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UProjectileMovementComponent* projectileMovementComponent;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInstance* materialBounced;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float allowerdDistanceBetweenPlayerAndProjectile;

	float distanceTravelled{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float allowedDistanceTravelled;

	bool materialChanged{ false };

	UFUNCTION()
		void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	



};
