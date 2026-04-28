// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SnakePlayerState.generated.h"

UCLASS()
class BOXROVERSNAKE67_API ASnakePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASnakePlayerState();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snake")
	int32 PlayerScore;

	UFUNCTION(BlueprintCallable, Category = "Snake")
	void AddScore(int32 Points);
	
	
};
