#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
    Empty,
    Wall,
    Fruit
};

UCLASS()
class BOXROVERSNAKE67_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    AGridManager();

    FVector GetRandomUnoccupiedWorldPosition();

    FVector GetWorldPositionForGridCell(int32 X, int32 Y);

    UPROPERTY(EditAnywhere, Category = "Grid")
    int32 GridWidth = 12;

    UPROPERTY(EditAnywhere, Category = "Grid")
    int32 GridHeight = 12;

    UPROPERTY(EditAnywhere, Category = "Grid")
    float TileSize = 100.0f; // Distance between walls in poopy unral units

protected:
    virtual void BeginPlay() override;

    // The Wall Blueprint to spawn
    UPROPERTY(EditAnywhere, Category = "Grid")
    TSubclassOf<AActor> WallClass;



    UPROPERTY(EditAnywhere, Category = "Grid")
	bool bGenerateOuterWalls = true;



    TArray<ETileType> GridData;

    // Helper to convert (X, Y) to a single array index
    int32 GetIndex(int32 X, int32 Y) const { return Y * GridWidth + X; }

    void GenerateGrid();


};