// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeTailSegment.h"

// Sets default values
ASnakeTailSegment::ASnakeTailSegment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASnakeTailSegment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASnakeTailSegment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

