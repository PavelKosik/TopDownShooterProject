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
	//waypointPrefab = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Waypoint Prefab"));	
	staticMeshPlacer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Placer"));
	//foundPlayer = false;
	RootComponent = enemyBody;
}

void AMyEnemyCharacter::OnConstruction(const FTransform& Transform) {

	Super::OnConstruction(Transform);

	if (shouldSpawnwaypoint) {
		waypointPositons.Add(staticMeshPlacer->GetComponentLocation());
		UE_LOG(LogTemp, Warning, TEXT("Everything works as intended so far"));


		shouldSpawnwaypoint = false;
	}





	/*	for (int i = 0; i < numberOfWaypoints; i++) {
			FName newName = FName(TEXT("Static Mesh Try "), i);
			UStaticMeshComponent* tryStatic = NewObject<UStaticMeshComponent>(this, newName);
			tryStatic->RegisterComponent();
			tryStatic->SetStaticMesh(staticMeshCube);
			tryStatic->SetupAttachment(RootComponent);
			tryStatic->SetRelativeLocation(FVector(10, 0, 0));
			//tryStatic->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

			tryStatic->CreationMethod = EComponentCreationMethod::Native;
			//staticMeshTry = CreateDefaultSubobject<UStaticMeshComponent>(newName);

			pathWaypoint.Emplace(tryStatic);

			UE_LOG(LogTemp, Warning, TEXT("%d"), i);

		}*/

}



// Called when the game starts or when spawned
void AMyEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	//inserts the start position so you dont have to do it by hand
	//TESTwaypointPositons.Insert(GetActorLocation(), 0);

	numberOfWaypoints = waypointPositons.Num();

	proceduralMesh->SetGenerateOverlapEvents(true);

	enemyBody = GetMesh();


	/*TArray<FVector> vertices;
	//X axis = lenght Y axis = angle Z = height
	vertices.Add(FVector(400, 150, -70)+GetActorLocation());
	vertices.Add(FVector(0, 10, -70) + GetActorLocation());
	vertices.Add(FVector(400, -150, -70) + GetActorLocation());

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

	proceduralMesh->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
	proceduralMesh->SetMaterial(0,materialMesh);
	*/

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

		//proceduralMesh->CreateMeshSection_LinearColor(i, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
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
		//DrawDebugSphere(GetWorld(), proceduralMesh->GetProcMeshSection(1)->SectionLocalBox.GetCenter(), 10, 26, FColor::Blue, false, -1, 0, 2);
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
				//UE_LOG(LogTemp, Warning, TEXT("%s"), *hit.GetActor()->GetFName().ToString());

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
			/*if (playerFoundThisFrame ==false) {
				foundPlayer = false;
			}*/
			//myVertices.Add(lineEndLocation);
			//DrawDebugLine(GetWorld(), FVector(lineStartLocation.X, lineStartLocation.Y, 30), FVector(lineEndLocation.X, lineEndLocation.Y, 30), FColor::FColor(materialMesh->GetMaterial()->BaseColor.MaskR, materialMesh->GetMaterial()->BaseColor.MaskG, materialMesh->GetMaterial()->BaseColor.MaskB, 1), false, -1, 0, 10);


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


				/*else {
					DrawDebugLine(GetWorld(), FVector(lineStartLocation.X, lineStartLocation.Y, 30), FVector(lineEndLocation.X, lineEndLocation.Y, 30), FColor::Blue, false, -1, 0, 10);
					UE_LOG(LogTemp, Warning, TEXT("SHOULD LINES DISSAPEAR NOW"));

				}*/
			}
		}

	}

	/*if (playerFoundThisFrame == false) {
		foundPlayer = false;
	}*/

	/*TArray<int> indexes;
	for (int x = 1; x < 400; x++) {

		FVector currentForwardVector = GetActorForwardVector() + FVector(0, 0, 0.1f);
		FVector currentAngleDirection = currentForwardVector.RotateAngleAxis(1.5f * x, FVector(0, 0, 1));

		FVector point = enemyBody->GetComponentLocation() + FVector(0, 0, 100) + currentAngleDirection * 100;

		FHitResult hit;
		FCollisionQueryParams collisionParams;
		collisionParams.bTraceComplex = true;
		collisionParams.bFindInitialOverlaps = true;

		//collisionParams.AddIgnoredActor(this);

		//DrawDebugLine(GetWorld(), point, GetActorLocation() + FVector(0, 0, -1000), FColor::Blue, false, -1, 0, 1);
		bool gotHit = GetWorld()->LineTraceSingleByChannel(hit, point, GetActorLocation() + FVector(0, 0, -1000), ECollisionChannel::ECC_Visibility, collisionParams);



		//UE_LOG(LogTemp, Warning, TEXT("%s"), *hit.Actor->GetFName().ToString());

		if (hit.Component == proceduralMesh) {


			indexes.Add(x);
			FHitResult finalHit;
			FCollisionQueryParams finalCollisionParams;
			FVector start = FVector(GetActorLocation().X, GetActorLocation().Y, 25);
			FVector end = GetActorLocation() + currentAngleDirection * 500;

			FVector finalVerticiesPos = FVector(end.X, end.Y, 25);


			bool gotFinalHit = GetWorld()->LineTraceSingleByChannel(finalHit, start, finalVerticiesPos, ECollisionChannel::ECC_Visibility, collisionParams);

			if (gotFinalHit) {
				finalVerticiesPos = finalHit.ImpactPoint;
				finalVerticiesPos = FVector(finalVerticiesPos.X, finalVerticiesPos.Y, 25);

			}

		}
	}
	*/
	//UE_LOG(LogTemp, Warning, TEXT("indexes: %d"), indexes.Num());



	//myVertices.Empty();

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

					UE_LOG(LogTemp, Warning, TEXT("Nalezen"));
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


