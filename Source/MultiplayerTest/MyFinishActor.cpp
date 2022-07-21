// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFinishActor.h"

// Sets default values
AMyFinishActor::AMyFinishActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	finishBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Finish Body Mesh"));
}

// Called when the game starts or when spawned
void AMyFinishActor::BeginPlay()
{

	Super::BeginPlay();

	finishBody->OnComponentBeginOverlap.AddDynamic(this, &AMyFinishActor::OnOverlapBegin);
	
}

// Called every frame
void AMyFinishActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyFinishActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->ActorHasTag("Player")) {
		AMyPlayer* player = Cast<AMyPlayer>(OtherActor);
		player->hasWon = true;
	}

}

