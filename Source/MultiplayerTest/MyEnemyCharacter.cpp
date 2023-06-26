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

	if (shouldSpawnwaypoint) {
		waypointPositons.Add(staticMeshPlacer->GetComponentLocation());
		shouldSpawnwaypoint = false;
	}
}



// Called when the game starts or when spawned
void AMyEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	numberOfWaypoints = waypointPositons.Num();

	proceduralMesh->SetGenerateOverlapEvents(true);

	enemyBody = GetMesh();



	for (int i = 0; i < 20; i++) {

		TArray<FVector> vertices;
		//X axis = lenght, Y axis = angle, Z = height
		vertices.Add(FVector(400 + enemyBody->GetRelativeLocation().X, 15 * i - enemyBody->GetRelativeLocation().Y, -50));
		vertices.Add(FVector(enemyBody->GetRelativeLocation().X, enemyBody->GetRelativeLocation().Y, -50));
		vertices.Add(FVector(400 + enemyBody->GetRelativeLocation().X, -15 * i - enemyBody->GetRelativeLocation().Y, -50));


		TArray<int32> Triangles;
		Triangles.Add(0);
		Triangles.Add(2);
		Triangles.Add(1);




		TArray<FVector> normals;
		normals.Add(FVector(0, 0, 1));
		normals.Add(FVector(0, 0, 1));
		normals.Add(FVector(0, 0, 1));

		TArray<FVector2D> UV0;
		UV0.Add(FVector2D(0, 0));
		UV0.Add(FVector2D(0, 0));
		UV0.Add(FVector2D(0, 0));


		TArray<FProcMeshTangent> tangents;
		tangents.Add(FProcMeshTangent(0, 1, 0));
		tangents.Add(FProcMeshTangent(0, 1, 0));
		tangents.Add(FProcMeshTangent(0, 1, 0));

		TArray<FLinearColor> vertexColors;
		vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
		vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
		vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));

		proceduralMesh->SetMaterial(i, materialMesh);
	}

	AMyEnemyCharacter::HandleRespawn();

}

// Called every frame
void AMyEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetWorld()->bBegunPlay == true) {

		playerFoundThisFrame = false;
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

				if (hit.GetActor()->GetFName().ToString() == "MyPlayer_BP_C_0") {

					foundPlayer = true;
					playerFoundThisFrame = true;
					myCont->lookingForPlayer = false;
					lastPlayerSawPos = hit.Location;
					player = Cast<AMyPlayer>(hit.GetActor());

				}

				if (hit.GetActor()->ActorHasTag("PlayerProjectile")) {
					myCont->sawBullet = true;
					UE_LOG(LogTemp, Warning, TEXT("SAW BULLET HIT"));
					myCont->lastBulletPos = hit.Location;
				}
			}

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


	if (displayWaypoints) {
		for (int i = 0; i < waypointPositons.Num(); i++) {
			DrawDebugBox(GetWorld(), waypointPositons[i], FVector(50, 50, 50), FColor::Purple, false, -1, 0, 10);
			//	UE_LOG(LogTemp, Warning, TEXT("Box should be drawn %d"), i);
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

	myCont = Cast<AMyAIController>(GetController());
	if (myCont) {
		myCont->myEnemyLogic = this;
	}
}

void AMyEnemyCharacter::HandleRespawn() {

	UMyGameInstance* gameInstance = Cast< UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (gameInstance != nullptr) {

		if (gameInstance->mySaveGame != nullptr) {
						
			bool found = false;
			
			for (int i = 0; i < gameInstance->mySaveGame->aliveEnemiesNames.Num(); i++) {

				if (gameInstance->mySaveGame->aliveEnemiesNames[i]==(GetFName()).ToString()) {
					found = true;
					return;				
				}

			}

			if (!found) {							
				Destroy();
			}
		}

	}

}


