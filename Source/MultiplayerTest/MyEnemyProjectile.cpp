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

	//sets the default projectiles values so it behaves the same way as the player projectile when spawned
	projectileBody->OnComponentHit.AddDynamic(this, &AMyEnemyProjectile::OnCollision);
	projectileMovementComponent->SetUpdatedComponent(projectileBody);
	projectileMovementComponent->InitialSpeed = projectileMoveSpeed;
	projectileMovementComponent->MaxSpeed = projectileMoveSpeed;
	projectileMovementComponent->bShouldBounce = true;
	projectileMovementComponent->Bounciness = 1.0f;
	projectileMovementComponent->ProjectileGravityScale = 0.0f;
	projectileMovementComponent->Velocity = positionToGoTo * projectileMovementComponent->InitialSpeed;

}

// Called every frame
void AMyEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//player has only 1 health so every time he collides with enemy projectile he should die
	if (collidedWithPlayer) {
		//makes sure that the projectile stops moving after it kills the player
		projectileMovementComponent->Velocity = FVector(0, 0, 0);

		if (GetWorldTimerManager().IsTimerActive(timerHandle) == false) {
			//sets a timer after which player is killed 
			GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AMyEnemyProjectile::KillPlayer, deathTimeDuration, false);
		}

	}

	else {
		//if projectlie goes a long time without hitting a player it's destroyed
		travelledDistance += projectileMoveSpeed;
		if (travelledDistance >= travelledDistanceMax) {
			Destroy();
		}
		//once parried the projectile follows different behaviour
		if (parried) {
			return;
		}
		//if the projectile has yet to bounce it just goes in a line it was shot at
		if (bounced == false) {
			projectileMovementComponent->Velocity = positionToGoTo * projectileMovementComponent->InitialSpeed;
		}


	}

}


void AMyEnemyProjectile::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag("Player")) {
		AMyPlayer* myPlayer = Cast<AMyPlayer>(OtherActor);
		//if player is hit and he isn't currently parrying then he is considered hit and the kill timer is started
		if (myPlayer->currentParryTime <= 0.0f) {
			myPlayer->hasBeenHit = true;
			AMyEnemyProjectile::StartKillPlayerTimer();
		}

		else {
			//if the player is parrying then there is a need to check if the projectile collided with the parry shield
			//if it didn't that would mean that player was parrying into the wrong direction
			if (OtherComp->ComponentHasTag("ParryShield")) {
				//after the parry the projectile travel distance is reseted 
				//in case the travelledDistance would be so close to max value that the projectile would be destroyed right after parry
				if (travelDistanceResetedAfterParry==false) {
					travelledDistance = 0.0f;
					travelDistanceResetedAfterParry = true;
				}
				//makes sure the projectile follows the direction it was parried in
				parried = true;
				bounced = false;
				projectileMovementComponent->Velocity = (myPlayer->cursorStaticMesh->GetRightVector()) * 1000 * projectileMovementComponent->InitialSpeed;
			}
			//player is considered hit if he parried in the wrong direction
			else {
				myPlayer->hasBeenHit = true;
				AMyEnemyProjectile::StartKillPlayerTimer();
			}
		}
	}


	else if (OtherActor->ActorHasTag("Enemy")) {
		//if projectile isn't parried but collides with the enemy it is destroyed 
		//because the projectiles can't pass through enemies and it would just get stuck next to the enemy
		if (!parried) {
			Destroy();
			return;
		}

		//if enemy collides with a parried projectile he is killed and the projectile is destroyed
		if (parried) {
			AActor* aiActor = OtherActor;
			AMyEnemyCharacter* ai = Cast<AMyEnemyCharacter>(aiActor);
			ai->myCont->killed = true;
			OtherActor->Destroy();
			Destroy();
		}
	}

	else {
		//if projectile didn't hit player nor enemy then it must have hit a wall and in that case it's considered bounced
		bounced = true;
	}

}

void AMyEnemyProjectile::StartKillPlayerTimer() {
	//makes sure that the collidedWithPlayer is set in case the timer wasn't set
	if (GetWorldTimerManager().IsTimerActive(timerHandle) == false) {
		collidedWithPlayer = true;
	}

}

void AMyEnemyProjectile::KillPlayer() {
	//destroys the player
	playerObject->Die();
	Destroy();
}



