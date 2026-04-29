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
    // 1. Prepare the array size
    GridData.SetNum(GridWidth * GridHeight);

    // 2. Initialize all tiles to Empty
    for (int32 i = 0; i < GridData.Num(); i++)
    {
        GridData[i] = ETileType::Empty;
    }

    // 3. Example: Add a border of walls
    for (int y = 0; y < GridHeight; y++)
    {
        for (int x = 0; x < GridWidth; x++)
        {
            if (x == 0 || x == GridWidth - 1 || y == 0 || y == GridHeight - 1)
            {
                GridData[GetIndex(x, y)] = ETileType::Wall;

                // Spawn the actual actor for the wall
                FVector SpawnLocation = GetActorLocation() + FVector(x * TileSize, y * TileSize, TileSize / 2);
                SpawnLocation -= FVector((GridWidth * TileSize) / 2 - TileSize / 2, (GridHeight * TileSize) / 2 - TileSize / 2, 0); // Center the grid around the origin
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AActor* NewWall = GetWorld()->SpawnActor<AActor>(WallClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

				if (NewWall)
				{
					NewWall->Tags.Add(FName("Wall")); // Add the wall tag so the Snake knows like those who knows
				}
            }
        }
    }
}

FVector AGridManager::GetRandomUnoccupiedWorldPosition()
{
    // Reservoir sampling: O(n) time, O(1) extra memory.
    int32 ChosenIndex = -1;
    int32 Count = 0;

    for (int32 i = 0; i < GridData.Num(); ++i)
    {
        if (GridData[i] == ETileType::Empty)
        {
            ++Count;
            // With probability 1/Count replace the current choice
            if (FMath::FRand() < 1.0f / static_cast<float>(Count))
            {
                ChosenIndex = i;
            }
        }
    }

    if (ChosenIndex < 0)
    {
        // No free tile found
        return FVector::ZeroVector;
    }

    // Convert linear index back to (x,y)
    int32 X = ChosenIndex % GridWidth;
    int32 Y = ChosenIndex / GridWidth;

    // Match the SpawnLocation centering logic from GenerateGrid()
    FVector WorldPos = GetActorLocation()
        + FVector(X * TileSize, Y * TileSize, TileSize / 2)
        - FVector((GridWidth * TileSize) / 2 - TileSize / 2,
                  (GridHeight * TileSize) / 2 - TileSize / 2,
                  0.0f);

    return WorldPos;
}

FVector AGridManager::GetWorldPositionForGridCell(int32 X, int32 Y)
{
    // Match the SpawnLocation centering logic from GenerateGrid()
    FVector WorldPos = GetActorLocation()
        + FVector(X * TileSize, Y * TileSize, TileSize / 2)
        - FVector((GridWidth * TileSize) / 2 - TileSize / 2,
                  (GridHeight * TileSize) / 2 - TileSize / 2,
                  0.0f);
    return WorldPos;
}
