// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "MyEnemyCharacter.h"
#include "MyPlayer.h"
#include "MyEnemyProjectile.h"

void AMyAIController::BeginPlay() {
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;
	myPawn = GetPawn();
	navMesh = UNavigationSystemV1::GetCurrent(this);
	//makes sure enemy follows the path that was created for him
	AMyAIController::FollowPath();

	//setups the default enemy settings
	player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	player->aiController = this;
	distanceToStartShootingAt = myEnemyLogic->distanceEnemyCanSee;
	//if enemy has no path to follow he is a stationary type of enemy
	if (myEnemyLogic->waypointPositons.Num() == 0) {
		myEnemyLogic->waypointPositons.Add(myEnemyLogic->GetActorLocation());
		//the rotation is important because this type of enemy should always have this rotation when he returns back to this position
		staticEnemyRotation = myEnemyLogic->GetActorRotation();
	}
}

void AMyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	deltaTime = DeltaTime;

	//stationary enemies rotate around the position they are meant to hold
	if (myEnemyLogic->waypointPositons.Num() <= 1) {
		//the enemy shouldn't rotate is he is currently looking for player or if the rotation already started
		if (lookingForPlayer || startedRotation) {
			return;
		}
		//if there is no reference to the logic or enemy is currently interacting with the player bullet he shouldn't rotate either
		if (myEnemyLogic == nullptr || goingToLastSawPosition || sawBullet) {
			return;
		}
		//if enemy is interacting with the player he shouldn't rotate either
		if (myEnemyLogic->foundPlayer || myEnemyLogic->playerFoundThisFrame) {
			return;
		}
		//if enemy is currently walking somewhere then he shouldn't rotate either
		if (IsFollowingAPath()) {
			return;
		}
		//if enemy is currently returning to his point of origin then he shouldn't rotate either
		if (returningToStationaryPoint) {
			return;
		}

		//if all the conditions are met then the enemy can proceed to rotate
		AMyAIController::RotateStationaryEnemy();
	}


	if (player) {
		//makes sure to check if enemy can hear player when he makes a sound
		if (player->madeSoundShoot || player->madeSoundStep) {
			AMyAIController::HandleHearing();
		}
	}


	currentTimeBetweenShots += DeltaTime;

	//if player is close enough to the enemy for him to get shot
	//the enemy stops, looks in the player direction and proceeds to shoot at player
	if (FVector::DistXY(myEnemyLogic->GetActorLocation(), myEnemyLogic->lastPlayerSawPos) < distanceToStartShootingAt && myEnemyLogic->playerFoundThisFrame) {
		PauseMove(GetCurrentMoveRequestID());
		AMyAIController::LookAtPlayer();
		AMyAIController::ShootPlayer();
	}

	//if enemy has seen player but can't see him this frame he proceeds to look for player
	if (myEnemyLogic->foundPlayer && myEnemyLogic->playerFoundThisFrame == false) {
		AMyAIController::WalkToLastSeenPosition();
		lookingForPlayer = true;
		shooting = false;
	}

	//if enemy couldn't find player the number of projectiles he shot gets reseted
	//and the shooting is set to false to reflect that enemy is no longer attacking player
	if (myEnemyLogic->foundPlayer == false) {
		numberOfShotProjectiles = 0;
		shooting = false;
		//if enemy sees bullet and can't see player he goes straight to the direction where he saw the bullet
		if (sawBullet) {
			if (goingToBulletLastSawPos == false) {
				StopMovement();
				dirToContinueWalking = lastBulletPos - myEnemyLogic->GetActorLocation();
				dirToContinueWalking.Normalize();
				MoveToLocation(lastBulletPos);
				goingToBulletLastSawPos = true;
			}
		}
	}

	//when enemy arrives at the position we was meant to look for player at
	//we tries to look around in order to find player
	if (lookingForPlayer) {
		if (IsFollowingAPath() == false) {
			StopMovement();
			AMyAIController::RotateAroundSearch();
		}


	}

	if (goingToBulletLastSawPos) {
		return;
	}

}

void AMyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	Super::OnMoveCompleted(RequestID, Result);

	//rotates the stationary enemy and makes sure he knows that he returned to the stationary point
	if (myEnemyLogic->waypointPositons.Num() <= 1) {
		returningToStationaryPoint = false;
		AMyAIController::RotateStationaryEnemy();
	}

	if (lookingForPlayer) {
		return;
	}

	if (myEnemyLogic == nullptr || goingToLastSawPosition || sawBullet) {
		return;
	}

	if (myEnemyLogic->foundPlayer == false) {

		if (finishedPath == false) {
			//if there is another point in the path this way
			//enemy proceeds to walk to such point
			if (currentPathIndex + 1 < myEnemyLogic->waypointPositons.Num()) {
				currentPathIndex += 1;
				AMyAIController::FollowPath();
				return;
			}

			//if there isn't such a point enemy proceeds to turn around and walk the same path but in reverse
			else {
				finishedPath = true;
				currentPathIndex -= 1;
				GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AMyAIController::Wait, maxWaitTime, false);
				return;
			}
		}

		if (finishedPath) {
			//checks if there is another point enemy can walk to in this direction
			if (currentPathIndex - 1 >= 0) {
				currentPathIndex -= 1;
				AMyAIController::FollowPath();
				return;
			}
			//if there isn't such a point enemy proceeds to turn around and walk the same path but in reverse
			else {
				finishedPath = false;
				currentPathIndex += 1;
				GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AMyAIController::Wait, maxWaitTime, false);
				return;
			}
		}

	}

	if (!lookingForPlayer) {
		currentPathIndex = 0;
	}


}

void AMyAIController::MoveToPlayer() {
	//moves to the player position
	lookingForPlayer = true;
	MoveToLocation(myEnemyLogic->lastPlayerSawPos);
}

void AMyAIController::ShootPlayer() {

	shooting = true;
	if (currentTimeBetweenShots >= timeBetweenShotsMax) {
		FActorSpawnParameters parameters;
		parameters.Owner = this;

		FRotator rotator = FRotator(0, 0, 0);

		FCollisionQueryParams collisionParameters;


		FVector spawnPos = myEnemyLogic->projectileSpawn->GetComponentLocation();
		FVector positionToGo;

		positionToGo = myEnemyLogic->projectileSpawn->GetForwardVector() * 10000;

		FHitResult hit;
		FCollisionQueryParams collisionParams;

		collisionParams.AddIgnoredActor(this);

		bool gotHit = GetWorld()->LineTraceSingleByChannel(hit, myEnemyLogic->GetActorLocation(), positionToGo, ECollisionChannel::ECC_WorldDynamic, collisionParams);


		AMyAIController::LookAtPlayer();

		AMyEnemyProjectile* spawned = GetWorld()->SpawnActor<AMyEnemyProjectile>(projectileSpawnObject, spawnPos, rotator, parameters);

		//normalize the vector so the projectile speed isn't affected by where player clicked
		positionToGo.Normalize(1.0f);


		//send the direction information to projectile
		spawned->positionToGoTo = positionToGo;
		spawned->startPosition = myEnemyLogic->projectileSpawn->GetRelativeLocation();
		spawned->playerObject = myEnemyLogic->player;
		currentTimeBetweenShots = 0.0f;
		numberOfShotProjectiles += 1;


	}

	else {
		FVector playerPos = player->GetNavAgentLocation();
		FVector pawnPos = myPawn->GetActorLocation();


		playerPos = FVector(playerPos.X, playerPos.Y, 10);
		pawnPos = FVector(pawnPos.X, pawnPos.Y, 10);

		FRotator lookRotation = UKismetMathLibrary::FindLookAtRotation(pawnPos, playerPos);
		FRotator setRotation = FRotator(0, lookRotation.Yaw, 0);

		myPawn->SetActorRotation(setRotation);
	}
}

void AMyAIController::LookAtPlayer() {
	//makes enemy rotate towards the player position
	MoveToActor(myEnemyLogic->player);
	StopMovement();
}

void AMyAIController::WalkToLastSeenPosition() {
	//walks to the position enemy last saw player at
	MoveToLocation(myEnemyLogic->lastPlayerSawPos);
	myEnemyLogic->foundPlayer = false;
	goingToLastSawPosition = true;
}

void AMyAIController::HandleHearing() {

	float distance = FVector::Dist(player->GetActorLocation(), myPawn->GetActorLocation());
	bool setPosition = false;

	//based on how far the player is from the enemy calculates the position of the player
	if (distance <= maxHearDistanceShoot && player->madeSoundShoot) {
		playerHeardPosition = player->GetActorLocation() + FVector(FMath::FRandRange(distance / 1000, distance / 950), FMath::RandRange(distance / 1000, distance / 950), 0);
		setPosition = true;
	}

	if (distance <= maxHearDistanceStep && player->madeSoundStep) {
		playerHeardPosition = player->GetActorLocation();
		setPosition = true;
	}
	//if enemy was close enough to hear the player then he proceeds to move to the position he heard the player at
	if (setPosition) {
		MoveTo(playerHeardPosition);
		lookingForPlayer = true;
	}
}

void AMyAIController::FollowPath() {

	if (myEnemyLogic) {

		if (myPawn) {

			navMesh = UNavigationSystemV1::GetCurrent(this);
			if (navMesh) {
				//moves to the location of the waypoint if the enemy isn't stationary and the currentPathIndex is a valid index of path
				if (myEnemyLogic->waypointPositons.Num() > 0 && myEnemyLogic->waypointPositons.IsValidIndex(currentPathIndex)) {
					myEnemyLogic->foundPlayer = false;
					goingToLastSawPosition = false;
					MoveToLocation(FVector(myEnemyLogic->waypointPositons[currentPathIndex].X, myEnemyLogic->waypointPositons[currentPathIndex].Y, myPawn->GetNavAgentLocation().Z));
				}
			}

			else {
				navMesh = UNavigationSystemV1::GetCurrent(this);
				AMyAIController::FollowPath();
			}
		}

		else {
			myPawn = GetPawn();
			AMyAIController::FollowPath();
		}
	}

	else {
		AMyAIController::FollowPath();
	}
}

void AMyAIController::Wait() {

	AMyAIController::FollowPath();

}

void AMyAIController::RotateAroundSearch() {
	lookingForPlayer = true;
	//rotates the enemy around it's axis to see if player is around him
	if (startedRotation) {
		//if the rotation is the same as the start rotation
		//that means that the enemy rotated all the way around and should return to the path
		if (myPawn->GetControlRotation().Quaternion() == rotateAroundRotation) {
			howMuchEnemyAlreadyRotated = 0;
			startedRotation = false;
			goingToLastSawPosition = false;
			lookingForPlayer = false;
			AMyAIController::FollowPath();
		}

		//continues to rotate the enemy around it's axis
		else {
			FQuat QuatRotation = FQuat(FRotator(0, searchRotationSpeed * deltaTime, 0));
			myPawn->AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
			howMuchEnemyAlreadyRotated += searchRotationSpeed * deltaTime;
		}
	}

	//starts the enemy rotation
	else {
		rotateAroundRotation = myPawn->GetControlRotation().Quaternion();
		FQuat QuatRotation = FQuat(FRotator(0, searchRotationSpeed * deltaTime, 0));
		myPawn->AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
		howMuchEnemyAlreadyRotated += searchRotationSpeed * deltaTime;
		startedRotation = true;
	}

	//if nothing was found when the enemy finished rotating he proceeds to follow the path again
	if (howMuchEnemyAlreadyRotated > 360.0f) {
		howMuchEnemyAlreadyRotated = 0;
		startedRotation = false;
		goingToLastSawPosition = false;
		lookingForPlayer = false;
		AMyAIController::FollowPath();
	}
}

void AMyAIController::RotateStationaryEnemy() {
	//rotates the stationary enemy around
	FVector currentRotationVector = myPawn->GetControlRotation().Vector();
	FVector startRotationVector = staticEnemyRotation.Vector();

	float dotProduct = FVector::DotProduct(currentRotationVector, startRotationVector);
	if (dotProduct > 0.85f) {
		myPawn->SetActorRotation(staticEnemyRotation);
	}

	if (myPawn->GetControlRotation() != staticEnemyRotation) {
		FQuat QuatRotation = FQuat(FRotator(0, stationaryRotationSpeed, 0));
		myPawn->AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
	}
}