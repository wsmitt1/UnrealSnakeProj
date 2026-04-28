#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FruitSpawner.generated.h"

class AFruit;
class AGridManager;

UCLASS()
class BOXROVERSNAKE67_API AFruitSpawner : public AActor
{
    GENERATED_BODY()

public:
    AFruitSpawner();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<AFruit> FruitClass;

    // We store a reference to the grid manager
    UPROPERTY(EditInstanceOnly, Category = "Spawning")
    AGridManager* GridManager;

private:
    // Track the current fruit in the world
    UPROPERTY()
    AFruit* CurrentActiveFruit;

public:
    // This will be called by the Snake or a Timer
    UFUNCTION(BlueprintCallable)
    void SpawnNewFruit();
};