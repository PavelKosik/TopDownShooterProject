// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayer.h"
#include "MyProjectile.h"
#include "MyAIController.h"
#include "MyHUD.h"
#include "MyDashStaminaBar.h"
#include "MyGameInstance.h"
#include "MySaveGame.h"

// Sets default values
AMyPlayer::AMyPlayer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	playerSkeletalMesh = GetMesh();

	UCapsuleComponent* capsule = GetCapsuleComponent();
	SetRootComponent(capsule);

	cameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");

	cameraAttach = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	parryShield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Parry Shield static mesh"));

	projectileSpawn = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile spawn"));

	cursorStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cursor static mesh"));

	cameraComponent->AttachToComponent(cameraAttach, FAttachmentTransformRules::KeepRelativeTransform);

	cameraAttach->SetupAttachment(RootComponent);
	cameraAttach->bUsePawnControlRotation = true;
	cameraAttach->bEnableCameraLag = true;
}


// Called when the game starts or when spawned
void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	projectileSpawn->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	AMyPlayer::SetupPlayerController();

	// used to make the mouse cursor centered on the start of the game
	int x, y;
	myPlayerController->GetViewportSize(x, y);

	// sets lastMouseCorrectPos equal to the center point of the screen
	lastMouseCorrectPos = FVector(x / 2, y / 2, 0);

	// sets player mouse position to the lastMouseCorrectPos (center of the screen)
	myPlayerController->SetMouseLocation(lastMouseCorrectPos.X, lastMouseCorrectPos.Y);

	viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	cameraFollowPosition = GetActorLocation();

	myHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());

	//makes sure the dashStaminaBar has a reference to player to use for it's scale
	if (myHUD) {

		myHUD->dashStaminaBar->aMyPlayer = this;
	}

	UMyGameInstance* gameInstance = Cast< UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	//the player position gets saved on checkpoints
	//if there is such a save, then player position is set to the position of last checkpoint
	if (gameInstance->mySaveGame != nullptr) {
		SetActorLocation(gameInstance->mySaveGame->playerPos);
	}
}

// Called every frame
void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//makes sure that player is always set to Walking in case he would collide with things that can make him rotate
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling) {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	if (playerSkeletalMesh == nullptr) {
		playerSkeletalMesh = GetMesh();
	}

	//if enemies managed to kill the player
	//then the player is disabled and the screen with him being dead is shown
	if (hasBeenHit) {
		SetActorTickEnabled(false);
		APlayerController* myPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (myPlayer) {
			currentDashTime = 0.0f;
			playerSkeletalMesh->SetAllPhysicsLinearVelocity(FVector(0, 0, 0));
			DisableInput(myPlayer);
		}
	}

	// viewportSize sometimes doesnt get set in the first frame 
	// so if it wasn't set this sets it
	if (viewportSize.X == 0 && viewportSize.Y == 0) {
		viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	}

	// used to position mouse at the centre of the screen in first few frames
	if (mouseSetUp == false) {
		myPlayerController->SetMouseLocation(lastMouseCorrectPos.X, lastMouseCorrectPos.Y);
		numberBeforeMouseSetUp += 1;
	}

	// after first 5 frames the mouse can start moving freely
	// done this way because in UE 4 play mode the game lags at the start
	// and it sometimes caused mouse position bug
	if (numberBeforeMouseSetUp > 5) {
		mouseSetUp = true;
	}

	// draws shot and foot step hear distance around player
	if (aiController) {

		// draw it only if wanted, for testing
		if (showDrawShootHearDistance) {
			FQuat tempQuatShoot = GetOwner()->GetActorRotation().Quaternion() * FRotator(90, 0, 0).Quaternion();

			FRotator DesiredAngleZShoot = tempQuatShoot.Rotator();

			FVector XVectorShoot = DesiredAngleZShoot.RotateVector(FVector(0, 1, 0));

			FVector YVectorShoot = DesiredAngleZShoot.RotateVector(FVector(0, 0, 1));

			DrawCircle(GetWorld(), GetActorLocation(), XVectorShoot, YVectorShoot, FColor::Yellow, aiController->maxHearDistanceShoot, 50, false, -1, 0, 10);
		}

		// draw it only if wanted, for testing
		if (showDrawStepHearDistance) {

			FQuat tempQuatStep = GetOwner()->GetActorRotation().Quaternion() * FRotator(90, 0, 0).Quaternion();

			FRotator DesiredAngleZStep = tempQuatStep.Rotator();

			FVector XVectorStep = DesiredAngleZStep.RotateVector(FVector(0, 1, 0));

			FVector YVectorStep = DesiredAngleZStep.RotateVector(FVector(0, 0, 1));

			DrawCircle(GetWorld(), GetActorLocation(), XVectorStep, YVectorStep, FColor::Blue, aiController->maxHearDistanceStep, 50, false, -1, 0, 10);

		}
	}

	AMyPlayer::KeepPlayerInAir();

	AMyPlayer::HandleSoundTimers(DeltaTime);

	int x, y;
	myPlayerController->GetViewportSize(x, y);
	//makes sure that all the screen related stuff works when player changes screen size
	if (x != viewportSize.X || y != viewportSize.Y) {
		viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	}

	if (shouldFreeView) {
		float mouseX, mouseY;
		myPlayerController->GetMousePosition(mouseX, mouseY);
		//player should be able to move the camera only when he reaches the edges of the screen
		//to prevent him from moving the camera accidentaly
		if (mouseX / viewportSize.X > 0.8f || mouseX / viewportSize.X < 0.2f || mouseY / viewportSize.Y > 0.8f || mouseY / viewportSize.Y < 0.2f) {
			cameraFollowPosition.Y -= cameraSpeed * (mouseY / viewportSize.Y - 0.5f);
		}
		//camera can only reach so far from the player to prevent player from seeing all the level from start
		if (FVector::Dist(playerSkeletalMesh->GetComponentLocation(), cameraFollowPosition) > maxCameraDistance) {
			cameraFollowPosition = lastCameraCorrectPos;
		}

		else {
			lastCameraCorrectPos = cameraFollowPosition;
		}

		cameraAttach->SetWorldLocation(cameraFollowPosition);
	}

	//camera is locked to the top of the player when he doesn't move it
	else {
		cameraFollowPosition = GetActorLocation();
		cameraAttach->SetWorldLocation(GetActorLocation());
	}

	if (currentDashTime > 0.0f) {
		LaunchCharacter(FVector(dashDirection.X * dashSpeed, dashDirection.Y * dashSpeed, dashDirection.Z * dashSpeed), true, true);
		currentDashTime -= DeltaTime;
	}

	//prevents player from being able to spam the dash
	if (currentDelayBetweenDashes > 0.0f) {
		currentDelayBetweenDashes -= DeltaTime;
	}

	//spawns the parry shield which player uses to parry the enemy attacks
	if (currentParryTime > 0.0f) {
		parryShield->SetWorldLocation(projectileSpawn->GetComponentLocation() - FVector(0, 0, 0));
		parryShield->SetWorldRotation(FRotator(playerSkeletalMesh->GetComponentRotation().Pitch, playerSkeletalMesh->GetComponentRotation().Yaw, 0));
		parryShield->SetVisibility(true);
		currentParryTime -= DeltaTime;
	}

	//despawns the parry shield
	else {
		parryShield->SetWorldLocation(projectileSpawn->GetComponentLocation() - FVector(0, 0, 0));
		parryShield->SetWorldRotation(FRotator(playerSkeletalMesh->GetComponentRotation().Pitch, playerSkeletalMesh->GetComponentRotation().Yaw, 0));
		parryShield->SetVisibility(false);
	}

	//prevents player from being able to spam the parry
	if (currentDelayBetweenParry > 0.0f) {
		currentDelayBetweenParry -= DeltaTime;
	}

	if (playerSkeletalMesh) {
		cursorStaticMesh->SetWorldRotation(FRotator(playerSkeletalMesh->GetComponentRotation().Pitch, playerSkeletalMesh->GetComponentRotation().Yaw, 0));
	}

	AMyPlayer::RotatePlayerToMousePosition();
}

void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Horizontal", this, &AMyPlayer::MoveHorizontal);
	PlayerInputComponent->BindAxis("Vertical", this, &AMyPlayer::MoveVertical);
	PlayerInputComponent->BindAction("MouseLeftClick", IE_Pressed, this, &AMyPlayer::LeftClickFunction);
	PlayerInputComponent->BindAction("MouseRightButton", IE_Pressed, this, &AMyPlayer::SetCameraFreeViewTrue);
	PlayerInputComponent->BindAction("MouseRightButton", IE_Released, this, &AMyPlayer::SetCameraFreeViewFalse);
	PlayerInputComponent->BindAction("PlayerDash", IE_Pressed, this, &AMyPlayer::PlayerDash);
	PlayerInputComponent->BindAction("PlayerParry", IE_Pressed, this, &AMyPlayer::PlayerParry);
}



void AMyPlayer::MoveHorizontal(float horizontal)
{
	//player can only move when he isn't in a dash state
	if (currentDashTime <= 0.0f) {
		if (horizontal > 0) {
			//used for handling sound and enemy hearing sounds
			madeSoundTimerStep = 0.25f;

			//this stops player if he walked in different direction before this
			if (currentPlayerMovementSpeedX > 0) {
				currentPlayerMovementSpeedX = 0;
			}

			//player builds momentum until a certain threshold
			if (currentPlayerMovementSpeedX - increasePlayerMoveSpeed >= -maxPlayerMoveSpeed) {
				currentPlayerMovementSpeedX -= increasePlayerMoveSpeed;
			}

			else {
				currentPlayerMovementSpeedX = -maxPlayerMoveSpeed;
			}
		}

		if (horizontal < 0) {
			//used for handling sound and enemy hearing sounds
			madeSoundTimerStep = 0.25f;
			//this stops player if he walked in different direction before this
			if (currentPlayerMovementSpeedX < 0) {
				currentPlayerMovementSpeedX = 0;
			}

			//player builds momentum until a certain threshold
			if (currentPlayerMovementSpeedX + increasePlayerMoveSpeed <= maxPlayerMoveSpeed) {
				currentPlayerMovementSpeedX += increasePlayerMoveSpeed;
			}

			else {
				currentPlayerMovementSpeedX = maxPlayerMoveSpeed;
			}
		}

		if (horizontal == 0) {
			//when player stop holding movement keys he doesn't stop right away but loses momentum over time
			if (currentPlayerMovementSpeedX > 0) {

				if (currentPlayerMovementSpeedX - decreasePlayerMoveSpeed > 0) {
					currentPlayerMovementSpeedX -= decreasePlayerMoveSpeed;
				}

				else {
					currentPlayerMovementSpeedX = 0;
				}
			}
			//when player stop holding movement keys he doesn't stop right away but loses momentum over time
			if (currentPlayerMovementSpeedX < 0) {

				if (currentPlayerMovementSpeedX + decreasePlayerMoveSpeed < 0) {
					currentPlayerMovementSpeedX += decreasePlayerMoveSpeed;
				}

				else {
					currentPlayerMovementSpeedX = 0;
				}
			}
		}
		//moves player based on the current momentum
		if (playerSkeletalMesh) {
			AddMovementInput(FVector(currentPlayerMovementSpeedX, 0, 0), 1);
		}
	}
	moveDirectionX = -horizontal;
}

void AMyPlayer::MoveVertical(float vertical)
{
	//player can only move when he isn't in a dash state
	if (currentDashTime <= 0.0f) {
		if (vertical > 0) {

			//used for handling sound and enemy hearing sounds
			madeSoundTimerStep = 0.25f;

			//this stops player if he walked in different direction before this
			if (currentPlayerMovementSpeedY < 0) {
				currentPlayerMovementSpeedY = 0;
			}
			//player builds momentum until a certain threshold
			if (currentPlayerMovementSpeedY + increasePlayerMoveSpeed <= maxPlayerMoveSpeed) {
				currentPlayerMovementSpeedY += increasePlayerMoveSpeed;
			}

			else {
				currentPlayerMovementSpeedY = maxPlayerMoveSpeed;
			}
		}

		if (vertical < 0) {
			//used for handling sound and enemy hearing sounds
			madeSoundTimerStep = 0.25f;

			//this stops player if he walked in different direction before this
			if (currentPlayerMovementSpeedY > 0) {
				currentPlayerMovementSpeedY = 0;
			}
			//player builds momentum until a certain threshold
			if (currentPlayerMovementSpeedY - increasePlayerMoveSpeed >= -maxPlayerMoveSpeed) {
				currentPlayerMovementSpeedY -= increasePlayerMoveSpeed;
			}

			else {
				currentPlayerMovementSpeedY = -maxPlayerMoveSpeed;
			}
		}

		if (vertical == 0) {
			//when player stop holding movement keys he doesn't stop right away but loses momentum over time
			if (currentPlayerMovementSpeedY > 0) {

				if (currentPlayerMovementSpeedY - decreasePlayerMoveSpeed > 0) {
					currentPlayerMovementSpeedY -= decreasePlayerMoveSpeed;
				}

				else {
					currentPlayerMovementSpeedY = 0;
				}
			}
			//when player stop holding movement keys he doesn't stop right away but loses momentum over time
			if (currentPlayerMovementSpeedY < 0) {

				if (currentPlayerMovementSpeedY + decreasePlayerMoveSpeed < 0) {
					currentPlayerMovementSpeedY += decreasePlayerMoveSpeed;
				}

				else {
					currentPlayerMovementSpeedY = 0;
				}
			}
		}
		//moves player based on the current momentum
		if (playerSkeletalMesh) {
			AddMovementInput(FVector(0, currentPlayerMovementSpeedY, 0), 1);
		}
	}

	moveDirectionY = vertical;
}

void AMyPlayer::LeftClickFunction()
{

	FHitResult hit;
	//player can only shoot if there isn't any other projectile spawned
	if (currentProjectile == NULL) {
		madeSoundTimerShoot = 0.25f;
		//check if player clicked somewhere on the map
		if (myPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, hit))
		{

			if (FVector::Dist2D(hit.ImpactPoint, playerSkeletalMesh->GetComponentLocation()) < 40) {
				return;
			}

			//else spawn the projectile
			else
			{
				FActorSpawnParameters parameters;
				parameters.Owner = this;

				FRotator rotator = FRotator(0, 0, 0);

				FCollisionQueryParams collisionParameters;

				//get projectile spawn position so we can spawn the projectile there
				FVector spawnPos = FVector(projectileSpawn->GetComponentLocation().X, projectileSpawn->GetComponentLocation().Y, GetActorLocation().Z);
				FVector positionToGo;

				positionToGo = (cursorStaticMesh->GetRightVector()) * 1000;
				positionToGo = FVector(positionToGo.X, positionToGo.Y, projectileSpawn->GetComponentLocation().Z);


				AMyProjectile* spawned = GetWorld()->SpawnActor<AMyProjectile>(projectileSpawnObject, spawnPos, rotator, parameters);

				currentProjectile = spawned;

				//normalize the vector so the projectile speed isn't affected by how far player clicked
				positionToGo.Normalize(1.0f);

				//send the direction information to projectile
				spawned->positionToGoTo = positionToGo;
				spawned->startPosition = projectileSpawn->GetRelativeLocation();
				spawned->playerObject = this;
			}

		}

		else {
			FActorSpawnParameters parameters;
			parameters.Owner = this;

			FRotator rotator = FRotator(0, 0, 0);

			FCollisionQueryParams collisionParameters;

			//get projectile spawn position so we can spawn the projectile there
			FVector spawnPos = projectileSpawn->GetComponentLocation();
			FVector positionToGo;

			positionToGo = (cursorStaticMesh->GetRightVector()) * 1000;
			positionToGo = FVector(positionToGo.X, positionToGo.Y, projectileSpawn->GetComponentLocation().Z);

			AMyProjectile* spawned = GetWorld()->SpawnActor<AMyProjectile>(projectileSpawnObject, spawnPos, rotator, parameters);

			currentProjectile = spawned;

			//normalize the vector so the projectile speed isn't affected by where player clicked
			positionToGo.Normalize(1.0f);


			//send the direction information to projectile
			spawned->positionToGoTo = positionToGo;
			spawned->startPosition = projectileSpawn->GetRelativeLocation();
			spawned->playerObject = this;

		}
	}
}

void AMyPlayer::RotatePlayerToMousePosition() {

	FVector mousePosition;

	FHitResult hit;

	//gets mouse position based on what's under cursor because the GetMousePosition functions didn't work as intended because of camera movement
	if (myPlayerController->GetHitResultUnderCursor(ECC_Visibility, true, hit)) {
		mousePosition = hit.Location;
	}

	if (playerSkeletalMesh) {
		FVector direction = mousePosition - (cameraAttach->GetComponentLocation() - playerSkeletalMesh->GetComponentLocation());

		FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(playerSkeletalMesh->GetComponentLocation(), direction);

		//decreasing the Y rotation by 90 so the player looks straight at the mouse position
		newRotation = newRotation - FRotator(0, 90, 0);
		newRotation = FRotator(0, newRotation.Yaw, 0);
		playerSkeletalMesh->SetWorldRotation(newRotation);
	}
}



void AMyPlayer::KeepPlayerInAir() {
	//makes sure the player doesn't change his Z position when he collides with stuff
	//because that would break the shooting and moving because of other collisions
	if (playerSkeletalMesh) {

		FVector oldPosition = playerSkeletalMesh->GetRelativeLocation();

		FVector newPosition = FVector(oldPosition.X, oldPosition.Y, 25.0f);

		playerSkeletalMesh->SetRelativeLocation(newPosition);
	}
}

//after player dies this reloads the level
void AMyPlayer::Die() {
	UGameplayStatics::OpenLevel(this, FName(GetWorld()->GetFName()), false);
}

//allows the player to use the camera to look around
void AMyPlayer::SetCameraFreeViewTrue() {
	shouldFreeView = true;
}

//after player stops looking around the camera snaps back to be on top of the player
void AMyPlayer::SetCameraFreeViewFalse() {
	shouldFreeView = false;
	cameraFollowPosition = GetActorLocation();
	cameraAttach->SetWorldLocation(GetActorLocation());
}

void AMyPlayer::SetupPlayerController() {

	// gets reference to the first player controller so we can set things up
	myPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	//show cursor so player can aim and shoot	
	myPlayerController->bShowMouseCursor = true;

	// enable all of mouse events
	myPlayerController->bEnableClickEvents = true;
	myPlayerController->bEnableMouseOverEvents = true;

	// reference to UUserWidget created from cursorWidgetTemplete
	// this UUserWidget is used as default game cursor
	UUserWidget* cursorWidget = CreateWidget<UUserWidget>(GetGameInstance(), cursorWidgetTemplete);

	// sets the default cursor to the cursor we created
	myPlayerController->SetMouseCursorWidget(EMouseCursor::Default, cursorWidget);

	// makes sure the mouse input works the same way when cursor is shown/not shown
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(myPlayerController, cursorWidget, EMouseLockMode::LockAlways, false);
}

void AMyPlayer::HandleSoundTimers(float DeltaTime) {

	// the way the sound timer system works is that
	// after player does some action enemies can hear (walk or shoot)
	// the system sets up a timer
	// if any enemy is in range of the action player made while the timer is not equal to 0
	// then the enemy heard the action

	if (madeSoundTimerShoot <= 0) {
		madeSoundShoot = false;
	}

	else {
		madeSoundShoot = true;
		madeSoundTimerShoot -= DeltaTime;
	}

	if (madeSoundTimerStep <= 0) {
		madeSoundStep = false;
	}

	else {
		madeSoundStep = true;
		madeSoundTimerStep -= DeltaTime;
	}

}

void AMyPlayer::PlayerDash() {

	//this delay makes sure that player can't spam the dash
	if (currentDelayBetweenDashes <= 0) {
		//sets the timers so make sure that the delays prevent player from using dash all the time
		currentDashTime = maxDashTime;
		currentDelayBetweenDashes = maxDelayBetweenDashes;

		//if player is currently moving in some direction he dashes in that direction
		if (moveDirectionX != 0 || moveDirectionY != 0) {

			dashDirection = FVector(moveDirectionX, moveDirectionY, 0);
			dashDirection.Normalize(1.0f);

		}

		//when player isn't moving he dashes in the direction he is currently facing
		if (moveDirectionX == 0 && moveDirectionY == 0) {

			dashDirection = (cursorStaticMesh->GetRightVector()) * 1000;
			dashDirection.Normalize(1.0f);

		}
	}
}

void AMyPlayer::PlayerParry() {
	//disallows player from spamming the parry
	if (currentDelayBetweenParry <= 0) {

		currentDelayBetweenParry = maxDelayBetweenParry;
		currentParryTime = parryDuration;

	}
}