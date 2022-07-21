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

	AMyAIController::FollowPath();
	// AMyAIController::HandleHearing();
	player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	player->aiController = this;
	distanceToStartShootingAt = myEnemyLogic->distanceEnemyCanSee;
	if (myEnemyLogic->waypointPositons.Num() == 0) {
		myEnemyLogic->waypointPositons.Add(myEnemyLogic->GetActorLocation());
		staticEnemyRotation = myEnemyLogic->GetActorRotation();
	}
}

void AMyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	deltaTime = DeltaTime;

	if (myEnemyLogic->waypointPositons.Num() <= 1) {

		if (lookingForPlayer == false && startedRotation == false) {

			if (myEnemyLogic && goingToLastSawPosition == false && sawBullet == false) {
				if (myEnemyLogic->foundPlayer == false && myEnemyLogic->playerFoundThisFrame == false) {

					if (IsFollowingAPath() == false) {
						//MoveToLocation(myEnemyLogic->waypointPositons[0]);
						if (returningToStationaryPoint == false) {
							AMyAIController::RotateStationaryEnemy();
						}
					}
				}
			}
		}
	}

	if (player) {

		if (player->madeSoundShoot || player->madeSoundStep) {
			AMyAIController::HandleHearing();
		}
	}


	currentTimeBetweenShots += DeltaTime;

	if (FVector::DistXY(myEnemyLogic->GetActorLocation(), myEnemyLogic->lastPlayerSawPos) < distanceToStartShootingAt && myEnemyLogic->playerFoundThisFrame) {

		PauseMove(GetCurrentMoveRequestID());
		AMyAIController::LookAtPlayer();
		AMyAIController::ShootPlayer();
	}


	if (myEnemyLogic->foundPlayer && myEnemyLogic->playerFoundThisFrame == false) {
		UE_LOG(LogTemp, Warning, TEXT("WALK TO LAST SEEN POSTION IF CALLED"));
		AMyAIController::WalkToLastSeenPosition();
		lookingForPlayer = true;
		shooting = false;
	}

	if (myEnemyLogic->foundPlayer == false) {
		numberOfShotProjectiles = 0;
		shooting = false;

		if (sawBullet) {
			UE_LOG(LogTemp, Warning, TEXT("SAW BULLET 1"));
			if (goingToBulletLastSawPos == false) {
				UE_LOG(LogTemp, Warning, TEXT("SAW BULLET 2"));
				StopMovement();
				dirToContinueWalking = lastBulletPos - myEnemyLogic->GetActorLocation();
				dirToContinueWalking.Normalize();
				MoveToLocation(lastBulletPos);
				goingToBulletLastSawPos = true;
			}


		}

	}

	if (lookingForPlayer) {
		if (IsFollowingAPath() == false) {
			StopMovement();
			//goingToLastSawPosition = false;
			UE_LOG(LogTemp, Warning, TEXT("ROTATION IF CALLED IN TICK"));
			AMyAIController::RotateAroundSearch();
		}


	}

	if (goingToBulletLastSawPos) {
		UE_LOG(LogTemp, Warning, TEXT("GOING TO LAST BULLET POST PART OF CODE CALLED IN TICK"));
		return;
	}



}

void AMyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	Super::OnMoveCompleted(RequestID, Result);

	if (myEnemyLogic->waypointPositons.Num() <= 1) {
		returningToStationaryPoint = false;
		AMyAIController::RotateStationaryEnemy();
	}

	if (lookingForPlayer == false) {

		if (myEnemyLogic && goingToLastSawPosition == false && sawBullet == false) {
			if (myEnemyLogic->foundPlayer == false) {

				if (finishedPath == false) {

					if (currentPathIndex + 1 < myEnemyLogic->waypointPositons.Num()) {
						currentPathIndex += 1;
						AMyAIController::FollowPath();
						return;
					}

					else {

						finishedPath = true;
						currentPathIndex -= 1;
						GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AMyAIController::Wait, maxWaitTime, false);
						return;
					}
				}

				if (finishedPath) {
					if (currentPathIndex - 1 >= 0) {
						currentPathIndex -= 1;
						AMyAIController::FollowPath();
						return;
					}

					else {

						finishedPath = false;
						currentPathIndex += 1;
						GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AMyAIController::Wait, maxWaitTime, false);
						return;
					}
				}

			}

			else {
				currentPathIndex = 0;
			}
		}
	}

	if (goingToBulletLastSawPos) {
		UE_LOG(LogTemp, Warning, TEXT("GOING TO LAST BULLET POST PART OF CODE CALLED"));
		returningToStationaryPoint = true;
		AMyAIController::FindPointNearLastSawBulletPos();
	}


}



/*void AMyAIController::MoveToRandomPoint() {
	//  myPawn = GetPawn();
	 // UE_LOG(LogTemp, Warning, TEXT("%s"), myEnemyLogic->foundPlayer? "true" : "false");
	if (myPawn) {
		navMesh = UNavigationSystemV1::GetCurrent(this);

		if (navMesh) {

			FNavLocation outLocation;

			float searchRadius = 350.0f;

			FNavLocation randomPt;

			const bool bFound = navMesh->GetRandomPointInNavigableRadius(myPawn->GetNavAgentLocation(), searchRadius, randomPt);

			if (bFound)
			{
				MoveToLocation(randomPt);
			}

			else {
				AMyAIController::MoveToRandomPoint();
			}

		}

		else {
			navMesh = UNavigationSystemV1::GetCurrent(this);
			AMyAIController::MoveToRandomPoint();

		}
	}

	else {
		myPawn = GetPawn();
		AMyAIController::MoveToRandomPoint();
	}
}
*/
void AMyAIController::MoveToPlayer() {

	lookingForPlayer = true;
	MoveToLocation(myEnemyLogic->lastPlayerSawPos);
}

void AMyAIController::ShootPlayer() {

	//AMyAIController::LookAtPlayer();
	shooting = true;
	//MAYBE WILL HELP lookingForPlayer = false;
	if (currentTimeBetweenShots >= timeBetweenShotsMax) {
		FActorSpawnParameters parameters;
		parameters.Owner = this;

		FRotator rotator = FRotator(0, 0, 0);

		FCollisionQueryParams collisionParameters;

		//get projectile spawn position so we can spawn the projectile there
		FVector spawnPos = myEnemyLogic->projectileSpawn->GetComponentLocation();
		//FVector positionToGo = FVector(hit.Location.X - projectileSpawn->GetComponentLocation().X, hit.Location.Y - projectileSpawn->GetComponentLocation().Y, 0);
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

	MoveToActor(myEnemyLogic->player);
	StopMovement();

}

void AMyAIController::WalkToLastSeenPosition() {

	MoveToLocation(myEnemyLogic->lastPlayerSawPos);
	myEnemyLogic->foundPlayer = false;
	goingToLastSawPosition = true;
	UE_LOG(LogTemp, Warning, TEXT("going to last saw position"));

}

void AMyAIController::HandleHearing() {

	float distance = FVector::Dist(player->GetActorLocation(), myPawn->GetActorLocation());
	bool setPosition = false;
	//UE_LOG(LogTemp, Warning, TEXT("%f"), distance);


	if (distance <= maxHearDistanceShoot && player->madeSoundShoot) {
		playerHeardPosition = player->GetActorLocation() + FVector(FMath::FRandRange(distance / 1000, distance / 950), FMath::RandRange(distance / 1000, distance / 950), 0);
		setPosition = true;
	}

	if (distance <= maxHearDistanceStep && player->madeSoundStep) {
		playerHeardPosition = player->GetActorLocation();
		setPosition = true;
	}

	/*if (FVector::Dist(player->GetActorLocation(), playerHeardPosition)>400.0f) {
		AMyAIController::HandleHearing();
	}*/




	/*  if (distance <= 200.0f) {
		  playerHeardPosition = player->GetActorLocation();
		  setPosition = true;
	  }

	  else if (distance <= 500.0f) {
		  playerHeardPosition = player->GetActorLocation() + FVector(FMath::FRandRange(10.0f, 15.0f), FMath::RandRange(10.0f, 15.0f), 0);
		  setPosition = true;

	  }

	  else if (distance <= 750.0f) {
		  playerHeardPosition = player->GetActorLocation() + FVector(FMath::FRandRange(15.0f, 20.0f), FMath::RandRange(10.0f, 15.0f), 0);
		  setPosition = true;

	  }

	  else if (distance <= 1250.0f) {
		  playerHeardPosition = player->GetActorLocation() + FVector(FMath::FRandRange(18.0f, 22.0f),FMath::RandRange(10.0f,15.0f),0);
		  setPosition = true;

	  }

	  else if (distance<=1500.0f) {
		  playerHeardPosition = player->GetActorLocation() + FVector(FMath::FRandRange(20.0f, 25.0f), FMath::RandRange(15.0f, 20.0f), 0);
		  setPosition = true;

	  }

	  else if (distance <= 2000.0f) {
		  playerHeardPosition = player->GetActorLocation() + FVector(FMath::FRandRange(25.0f, 35.0f), FMath::RandRange(15.0f, 20.0f), 0);
		  setPosition = true;

	  }

	  */


	  /*FNavLocation loc;
	  FVector extent = FVector(1, 1, 1); maybe try again*/
	  /*FNavAgentProperties* AgentProperties;
	  FSharedConstNavQueryFilter QueryFilter;*/
	  //if (navMesh->ProjectPointToNavigation(playerHeardPosition,loc,extent)) { maybe try again
	if (setPosition) {
		MoveTo(playerHeardPosition);
		lookingForPlayer = true;
	}
	//}


}

void AMyAIController::FollowPath() {

	if (myEnemyLogic) {


		if (myPawn) {

			navMesh = UNavigationSystemV1::GetCurrent(this);


			if (navMesh) {

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
	UE_LOG(LogTemp, Warning, TEXT("ROTATE AROUND AND SEARCH CALLED"));
	lookingForPlayer = true;
	if (startedRotation) {
		if (myPawn->GetControlRotation().Quaternion() == rotateAroundRotation) {
			howMuchEnemyAlreadyRotated = 0;
			startedRotation = false;
			goingToLastSawPosition = false;
			lookingForPlayer = false;
			UE_LOG(LogTemp, Warning, TEXT("SAME ROT"));
			AMyAIController::FollowPath();
		}

		else {
			FQuat QuatRotation = FQuat(FRotator(0, searchRotationSpeed * deltaTime, 0));
			myPawn->AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
			howMuchEnemyAlreadyRotated += searchRotationSpeed * deltaTime;
		}
	}

	else {
		rotateAroundRotation = myPawn->GetControlRotation().Quaternion();
		FQuat QuatRotation = FQuat(FRotator(0, searchRotationSpeed * deltaTime, 0));
		myPawn->AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
		howMuchEnemyAlreadyRotated += searchRotationSpeed * deltaTime;
		startedRotation = true;
	}

	if (howMuchEnemyAlreadyRotated > 360.0f) {
		howMuchEnemyAlreadyRotated = 0;
		startedRotation = false;
		goingToLastSawPosition = false;
		lookingForPlayer = false;
		AMyAIController::FollowPath();
	}
}

void AMyAIController::FindPointNearLastSawBulletPos() {
	FNavLocation randomPt;
	float searchRadius = 100.0f;
	FVector pos = (player->GetActorLocation() - myPawn->GetActorLocation() / 2);
	const bool bFound = true;//navMesh->GetRandomPointInNavigableRadius(pos, searchRadius, randomPt);
	//FVector randomPt1 = randomPt;


	if (bFound && IsFollowingAPath() == false) {
		//MoveToLocation(randomPt1);
		UE_LOG(LogTemp, Warning, TEXT("SAW BULLET 3"));
		goingToBulletLastSawPos = false;
		sawBullet = false;
		//UNCOMENT IF TEST DOES NOT WORK	AMyAIController::RotateAroundSearch();
	}



}

void AMyAIController::RotateStationaryEnemy() {
	FVector currentRotationVector = myPawn->GetControlRotation().Vector();
	FVector startRotationVector = staticEnemyRotation.Vector();

	float dotProduct = FVector::DotProduct(currentRotationVector, startRotationVector);
	if (dotProduct > 0.85f) {
		myPawn->SetActorRotation(staticEnemyRotation);
	}

	if (myPawn->GetControlRotation() != staticEnemyRotation) {
		FQuat QuatRotation = FQuat(FRotator(0, stationaryRotationSpeed, 0));
		myPawn->AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
		UE_LOG(LogTemp, Warning, TEXT("CALLED ROTATION STATIONARY"));
		//AMyAIController::RotateStationaryEnemy();
	}
}