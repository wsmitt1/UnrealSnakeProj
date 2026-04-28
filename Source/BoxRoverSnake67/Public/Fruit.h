#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Fruit.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class BOXROVERSNAKE67_API AFruit : public AActor
{
    GENERATED_BODY()

public:
    AFruit();
    UPROPERTY(EditAnywhere, Category = "Fruit Settings")
    // How much score this fruit gives
    int32 ScoreValue = 10;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* MeshComponent;



};