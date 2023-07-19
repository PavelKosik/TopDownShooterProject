// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyCharacter.h"
#include "MyAIController.h"
#include "MyPlayer.h"
#include "MyGameInstance.h"
#include "MySaveGame.h"
// Sets default values
AMyEnemyCharacter::AMyEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	proceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural mesh"));
	projectileSpawn = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile spawn"));
	arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("arrow comp"));
	staticMeshPlacer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Placer"));
	RootComponent = enemyBody;
}

void AMyEnemyCharacter::OnConstruction(const FTransform& Transform) {

	Super::OnConstruction(Transform);

	//used to create waypoints enemies follow when they aren't interacting with the player
	//shouldSpawnWaypoint is variable that can be seen in Unreal Engine Editor and it functions as a button
	if (shouldSpawnWaypoint) {
		waypointPositons.Add(staticMeshPlacer->GetComponentLocation());
		shouldSpawnWaypoint = false;
	}
}



// Called when the game starts or when spawned
void AMyEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	//gets reference to the number of waypoints
	numberOfWaypoints = waypointPositons.Num();

	enemyBody = GetMesh();

	AMyEnemyCharacter::HandleRespawn();
}

// Called every frame
void AMyEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if the game is currently running
	if (GetWorld()->bBegunPlay == true) {

		playerFoundThisFrame = false;

		//spawns the lines which show enemy field of view
		for (int i = -widthEnemyCanSee; i < widthEnemyCanSee; i++) {

			FHitResult hit;
			FCollisionQueryParams collisionParams;

			collisionParams.AddIgnoredActor(this);

			FVector currentActorForward = GetActorForwardVector();
			FVector currentAngleDirection = currentActorForward.RotateAngleAxis(distanceBetweenVisionLines * i, FVector(0, 0, 1));


			FVector lineStartLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 50);

			FVector lineEndLocation = GetActorLocation() + currentAngleDirection * distanceEnemyCanSee;
			lineEndLocation = FVector(lineEndLocation.X, lineEndLocation.Y, 50);

			bool gotHit = GetWorld()->LineTraceSingleByChannel(hit, lineStartLocation, lineEndLocation, ECollisionChannel::ECC_WorldDynamic, collisionParams);

			if (gotHit) {

				lineEndLocation = hit.ImpactPoint;
				//if the vision line collides with player then the player has been found and the enemy procced to engage a fight with him
				if (hit.GetActor()->GetFName().ToString() == "MyPlayer_BP_C_0") {

					foundPlayer = true;
					playerFoundThisFrame = true;
					myCont->lookingForPlayer = false;
					lastPlayerSawPos = hit.Location;
					player = Cast<AMyPlayer>(hit.GetActor());

				}
				//if enemy sees player projectile he procceds to walk to the position he saw the bullet in and looks around to see if he can find player
				if (hit.GetActor()->ActorHasTag("PlayerProjectile")) {
					myCont->sawBullet = true;
					myCont->lastBulletPos = hit.Location;
				}
			}

			//the vision lines change color to reflect the state of the enemy
			//the enemy has 3 states
			//attacking which is symbolized by the attackColor
			//patrol which is symbolized by the neutralColor
			//and searching for player which is symbolized by the searchColor
			if (myCont->shooting) {
				DrawDebugLine(GetWorld(), FVector(lineStartLocation.X, lineStartLocation.Y, 30), FVector(lineEndLocation.X, lineEndLocation.Y, 30), attackColor, false, -1, 0, 10);
			}

			else {

				if (foundPlayer == false && myCont->sawBullet == false) {
					DrawDebugLine(GetWorld(), FVector(lineStartLocation.X, lineStartLocation.Y, 30), FVector(lineEndLocation.X, lineEndLocation.Y, 30), neutralColor, false, -1, 0, 10);
				}

				if (myCont->goingToLastSawPosition || myCont->lookingForPlayer || myCont->sawBullet) {

					DrawDebugLine(GetWorld(), FVector(lineStartLocation.X, lineStartLocation.Y, 30), FVector(lineEndLocation.X, lineEndLocation.Y, 30), searchColor, false, -1, 0, 10);

				}

				if (foundPlayer) {
					DrawDebugLine(GetWorld(), FVector(lineStartLocation.X, lineStartLocation.Y, 30), FVector(lineEndLocation.X, lineEndLocation.Y, 30), searchColor, false, -1, 0, 10);
				}
			}
		}

	}
	//shows the waypoints so that the path enemy follows is visible
	//the game doesn't have to be running for this to work
	//it's a  function meant for easier level design
	if (displayWaypoints) {
		for (int i = 0; i < waypointPositons.Num(); i++) {
			DrawDebugBox(GetWorld(), waypointPositons[i], FVector(50, 50, 50), FColor::Purple, false, -1, 0, 10);
		}
	}
}


// Called to bind functionality to input
void AMyEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AMyEnemyCharacter::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AMyEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//makes sure the controller has reference to this object
	myCont = Cast<AMyAIController>(GetController());
	if (myCont) {
		myCont->myEnemyLogic = this;
	}
}

void AMyEnemyCharacter::HandleRespawn() {
	//checks if there is a save
	UMyGameInstance* gameInstance = Cast< UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (gameInstance != nullptr) {

		if (gameInstance->mySaveGame != nullptr) {
			bool found = false;
			//if there was a save found then it searchers through all the enemies which were saved as alive in the last save
			for (int i = 0; i < gameInstance->mySaveGame->aliveEnemiesNames.Num(); i++) {
				//if it manages to find itself then the enemy can be respawned
				if (gameInstance->mySaveGame->aliveEnemiesNames[i]==(GetFName()).ToString()) {
					found = true;
					return;				
				}

			}
			//if the enemy wasn't found then it must have been dead last save and therefore it isn't respawned
			if (!found) {							
				Destroy();
			}
		}

	}

}


