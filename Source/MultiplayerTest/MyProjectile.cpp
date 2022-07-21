// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectile.h"
#include "MyPlayer.h"
#include "MyAIController.h"
#include "MyEnemyCharacter.h"

// Sets default values
AMyProjectile::AMyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	projectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile mesh"));

	projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile movement component"));

	//material = CreateDefaultSubobject<UMaterialInstance>(TEXT("Material"));

	RootComponent = projectileMesh;
}

// Called when the game starts or when spawned
void AMyProjectile::BeginPlay()
{
	Super::BeginPlay();	

	projectileMesh->OnComponentHit.AddDynamic(this, &AMyProjectile::OnCollision);

	projectileMovementComponent->SetUpdatedComponent(projectileMesh);
	projectileMovementComponent->InitialSpeed = projectileMoveSpeed;
	projectileMovementComponent->MaxSpeed = projectileMoveSpeed;
	//projectileMovementComponent->bRotationFollowsVelocity = true;
	projectileMovementComponent->bShouldBounce = true;
	projectileMovementComponent->Bounciness = 1.0f;
	projectileMovementComponent->ProjectileGravityScale = 0.0f;
	//UE_LOG(LogTemp, Warning, TEXT("%f,%f,%f"), projectileMovementComponent->Velocity.X, projectileMovementComponent->Velocity.Y, projectileMovementComponent->Velocity.Z);
}

// Called every frame
void AMyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	distanceTravelled += projectileMoveSpeed;

	if (distanceTravelled>= allowedDistanceTravelled) {
		playerObject->currentProjectile = NULL;
		Destroy();
	}

	//if the projectile didn't bounce yet, it follows the direction it was shot in
	if (bounced==false) 
	{	
		projectileMovementComponent->Velocity = positionToGoTo * projectileMovementComponent->InitialSpeed;
		//UE_LOG(LogTemp, Warning, TEXT("%f,%f,%f"), projectileMovementComponent->Velocity.X, projectileMovementComponent->Velocity.Y, projectileMovementComponent->Velocity.Z);

	}


	else 
	{		
		if (materialChanged==false) {
			//UMaterial* mat = materialBounced->GetMaterial();
			projectileMesh->SetMaterial(0, materialBounced);
			materialChanged = true;
		}

		//makes sure the projectile doesn't fly away from the map
		projectileMesh->SetRelativeLocation(FVector(projectileMesh->GetRelativeLocation().X, projectileMesh->GetRelativeLocation().Y, 28.0f));	
	}

	//destroy the projectile so it doesn't keep bouncing forever
	//because that would make player not be able to shoot
	if (currentNumberOfBounces>= maximalNumberOfBounces) {
		playerObject->currentProjectile = NULL;
		Destroy();
	}

	//UE_LOG(LogTemp, Warning, TEXT("%f"), FVector::Distance(playerObject->playerMesh->GetComponentLocation(), projectileMesh->GetComponentLocation()));

	//if projectile is too far away, destroy it
	if (FVector::Distance(playerObject->playerMesh->GetComponentLocation(), projectileMesh->GetComponentLocation())> allowerdDistanceBetweenPlayerAndProjectile) {
		playerObject->currentProjectile = NULL;
		Destroy();
	}
}

void AMyProjectile::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	//if the hit actor is the wall we allow the projectile to bounce so it doesn't follow the shoot direction
	if (OtherActor->ActorHasTag("Wall")) {
		bounced = true;
		currentNumberOfBounces += 1;
	}
		
	
	if (OtherActor->ActorHasTag("Enemy")) {

		//enemy can be killed by player only if the projectile has already bounced
		if (bounced) {
			AActor* aiActor = OtherActor;
			AMyEnemyCharacter* ai = Cast<AMyEnemyCharacter>(aiActor);
			ai->myCont->killed = true;
			/*OtherActor->SetActorHiddenInGame(true);
			OtherActor->SetActorEnableCollision(false);*/
			//OtherActor->SetActorTickEnabled(false);
			OtherActor->Destroy();
			playerObject->currentProjectile = NULL;
			Destroy();
		}

		//else destroy the projectile
		else {
			playerObject->currentProjectile = NULL;
			Destroy();
		}
	}

	if (OtherActor->ActorHasTag("Ground")) {
		UE_LOG(LogTemp, Warning, TEXT("Hit the ground running"));
	}
}

