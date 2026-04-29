#include "FruitSpawner.h"
#include "Fruit.h"
#include "GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "SnakeGameInstance.h"

AFruitSpawner::AFruitSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AFruitSpawner::BeginPlay()
{
    Super::BeginPlay();
    
    // If you forgot to assign the GridManager in the editor, try to find it
    if (!GridManager)
    {
        GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
    }

    SpawnNewFruit();
}

void AFruitSpawner::SpawnNewFruit()
{
    // Ensure we only have one fruit. If one exists, don't spawn.
    // (Note: If the Snake destroys the actor, CurrentActiveFruit becomes null automatically if it's a UPROPERTY)
    if (IsValid(CurrentActiveFruit)) return;

    USnakeGameInstance* GI = Cast<USnakeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    if (GridManager && FruitClass && ExtremeFruit)
    {
        FVector SpawnPos = GridManager->GetRandomUnoccupiedWorldPosition();

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        CurrentActiveFruit = GetWorld()->SpawnActor<AFruit>((GI->Difficulty == "Extreme" ? ExtremeFruit : FruitClass) , SpawnPos, FRotator::ZeroRotator, Params);
    }
}