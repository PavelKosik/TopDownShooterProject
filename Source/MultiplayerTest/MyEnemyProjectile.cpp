// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyProjectile.h"
#include "MyPlayer.h"
#include "MyEnemyCharacter.h"
#include "MyAIController.h"

// Sets default values
AMyEnemyProjectile::AMyEnemyProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	projectileBody = CreateDefaultSubobject<UStaticMeshComponent>("Projectile Body");

	projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile movement component"));
	RootComponent = projectileBody;
}

// Called when the game starts or when spawned
void AMyEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	projectileBody->OnComponentHit.AddDynamic(this, &AMyEnemyProjectile::OnCollision);
	projectileMovementComponent->SetUpdatedComponent(projectileBody);
	projectileMovementComponent->InitialSpeed = projectileMoveSpeed;
	projectileMovementComponent->MaxSpeed = projectileMoveSpeed;
	//projectileMovementComponent->bRotationFollowsVelocity = true;
	projectileMovementComponent->bShouldBounce = true;
	projectileMovementComponent->Bounciness = 1.0f;
	projectileMovementComponent->ProjectileGravityScale = 0.0f;
	projectileMovementComponent->Velocity = positionToGoTo * projectileMovementComponent->InitialSpeed;

}

// Called every frame
void AMyEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (collidedWithPlayer) {
		projectileMovementComponent->Velocity = FVector(0, 0, 0);

		if (GetWorldTimerManager().IsTimerActive(timerHandle) == false) {

			GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AMyEnemyProjectile::KillPlayer, deathTimeDuration, false);
		}

	}

	else {

		travelledDistance += projectileMoveSpeed;
		if (travelledDistance >= travelledDistanceMax) {
			Destroy();
		}

		if (parried) {
			return;
		}

		if (bounced == false) {
			projectileMovementComponent->Velocity = positionToGoTo * projectileMovementComponent->InitialSpeed;
		}


	}

}


void AMyEnemyProjectile::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag("Player")) {
		AMyPlayer* myPlayer = Cast<AMyPlayer>(OtherActor);

		if (myPlayer->currentParryTime <= 0.0f) {
			myPlayer->hasBeenHit = true;
			AMyEnemyProjectile::StartKillPlayerTimer();
		}

		else {

			if (OtherComp->ComponentHasTag("ParryShield")) {
				if (travelDistanceResetedAfterParry==false) {
					travelledDistance = 0.0f;
					travelDistanceResetedAfterParry = true;
				}
				parried = true;
				bounced = false;
				projectileMovementComponent->Velocity = (myPlayer->cursorStaticMesh->GetRightVector()) * 1000 * projectileMovementComponent->InitialSpeed;
			}

			else {
				myPlayer->hasBeenHit = true;
				AMyEnemyProjectile::StartKillPlayerTimer();
			}
		}
	}


	else if (OtherActor->ActorHasTag("Enemy")) {

		if (!parried) {
			Destroy();
			return;
		}


		if (bounced) {
			AActor* aiActor = OtherActor;
			AMyEnemyCharacter* ai = Cast<AMyEnemyCharacter>(aiActor);
			ai->myCont->killed = true;
			OtherActor->Destroy();
			Destroy();
		}

		else {
			Destroy();
		}


	}

	else {
		bounced = true;
	}

}

void AMyEnemyProjectile::StartKillPlayerTimer() {
	if (GetWorldTimerManager().IsTimerActive(timerHandle) == false) {
		collidedWithPlayer = true;
	}

}

void AMyEnemyProjectile::KillPlayer() {
	UE_LOG(LogTemp, Warning, TEXT("TIMER ACTIVE CALLED"));
	playerObject->Die();
	Destroy();
	/*if (GetWorldTimerManager().IsTimerActive(timerHandle) == false) {
		UE_LOG(LogTemp, Warning, TEXT("TIMER ACTIVE FALSE"));
		playerObject->Die();
		Destroy();
	}

	else {
		UE_LOG(LogTemp, Warning, TEXT("TIMER ACTIVE TRUE"));
		AMyEnemyProjectile::KillPlayer();
	}*/
}



