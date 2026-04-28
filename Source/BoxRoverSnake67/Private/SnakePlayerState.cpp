// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePlayerState.h"

ASnakePlayerState::ASnakePlayerState()
{
    // Always initialize your variables!
    PlayerScore = 0;
}

void ASnakePlayerState::AddScore(int32 Points)
{
    PlayerScore += Points;
    UE_LOG(LogTemp, Warning, TEXT("Player %s Score: %d"), *GetPlayerName(), PlayerScore);
}