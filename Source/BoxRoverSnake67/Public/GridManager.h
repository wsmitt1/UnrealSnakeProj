#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

UCLASS()
class BOXROVERSNAKE67_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    AGridManager();

protected:
    virtual void BeginPlay() override;

    // The Wall Blueprint to spawn
    UPROPERTY(EditAnywhere, Category = "Grid")
    TSubclassOf<AActor> WallClass;

    UPROPERTY(EditAnywhere, Category = "Grid")
    int32 GridWidth = 12;

    UPROPERTY(EditAnywhere, Category = "Grid")
    int32 GridHeight = 12;

    UPROPERTY(EditAnywhere, Category = "Grid")
    float TileSize = 100.0f; // Distance between walls in poopy unral units

    void GenerateGrid();
};