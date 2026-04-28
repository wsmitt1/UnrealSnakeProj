// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"

// Sets default values
AGridManager::AGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	GenerateGrid();
}

void AGridManager::GenerateGrid()
{
	if (!WallClass) return;
	for (int32 x = 0; x < GridWidth; x++)
	{
		for (int32 y = 0; y < GridHeight; y++)
		{
			if (bGenerateOuterWalls && (x == 0 || x == GridWidth - 1 || y == 0 || y == GridHeight - 1))
			{
				FVector Location(x * TileSize, y * TileSize, TileSize / 2);
				Location -= FVector((GridWidth * TileSize) / 2 - TileSize / 2, (GridHeight * TileSize) / 2 - TileSize / 2, 0); // Center the grid around the origin

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AActor* NewWall = GetWorld()->SpawnActor<AActor>(WallClass, Location, FRotator::ZeroRotator, SpawnParams);

				if (NewWall)
				{
					NewWall->Tags.Add(FName("Wall")); // Add the wall tag so the Snake knows like those who knows
				}
			}
		}
	}
}

