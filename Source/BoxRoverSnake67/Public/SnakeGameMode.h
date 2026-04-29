// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridManager.h"
#include "NewSnakePawn.h"
#include "GameFramework/GameModeBase.h"
#include "SnakeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BOXROVERSNAKE67_API ASnakeGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void StartMultiplayerMatch();
	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void StartSingleplayerMatch();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Snake")
	TSubclassOf<ANewSnakePawn> SnakeBlueprintClass;

	ANewSnakePawn* SpawnSnakeForPlayer(int32 PlayerIndex, FVector SpawnLocation);

	UPROPERTY(EditAnywhere, Category = "Snake")
	FVector2D Player1SpawnCell = FVector2D(5, 5);
	UPROPERTY(EditAnywhere, Category = "Snake")
	FVector2D Player2SpawnCell = FVector2D(15, 15);

private:
	AGridManager* myGridManager;
};
