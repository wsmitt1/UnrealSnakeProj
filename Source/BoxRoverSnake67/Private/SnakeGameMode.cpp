// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameMode.h"
#include "NewSnakePawn.h"
#include "GridManager.h"
#include "SnakeGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void ASnakeGameMode::BeginPlay() {
	Super::BeginPlay();

	USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
	if (GI) {
		if (GI->bIsMultiplayer) {
			StartMultiplayerMatch();
		}
		else {
			StartSingleplayerMatch();
		}
	}
}
void ASnakeGameMode::StartSingleplayerMatch() {
	UE_LOG(LogTemp, Warning, TEXT("Starting Singleplayer Match"));
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	
	myGridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	ANewSnakePawn* Snake = SpawnSnakeForPlayer(0, myGridManager->GetWorldPositionForGridCell(Player1SpawnCell.X, Player1SpawnCell.Y));
	if (PC && Snake) { PC->Possess(Snake); UE_LOG(LogTemp, Warning, TEXT("Player 0 Creation and Possession Successful"));
		// 1. Tell the controller to focus on the GAME, not the UI
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);

		// 2. Hide the mouse so it doesn't get in the way
		PC->bShowMouseCursor = false;
	}
}


void ASnakeGameMode::StartMultiplayerMatch() {
	myGridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));

	// 1. Create the second player FIRST
	// This forces Unreal to set up the Split Screen viewport before we spawn snakes
	APlayerController* PC1 = UGameplayStatics::GetPlayerController(this, 1);

	if (PC1 == nullptr) {
		// Only create if they don't exist
		PC1 = UGameplayStatics::CreatePlayer(this, 1, true);
	}
	APlayerController* PC0 = UGameplayStatics::GetPlayerController(this, 0);

	if (myGridManager && PC0 && PC1) {
		// 2. Spawn and Possess for Player 0
		ANewSnakePawn* Snake0 = SpawnSnakeForPlayer(0, myGridManager->GetWorldPositionForGridCell(Player1SpawnCell.X, Player1SpawnCell.Y));
		if (Snake0) {
			PC0->Possess(Snake0);
			// Force the view to the pawn immediately
			PC0->SetViewTargetWithBlend(Snake0);
		}

		// 3. Spawn and Possess for Player 1
		ANewSnakePawn* Snake1 = SpawnSnakeForPlayer(1, myGridManager->GetWorldPositionForGridCell(Player2SpawnCell.X, Player2SpawnCell.Y));
		if (Snake1) {
			PC1->Possess(Snake1);
			PC1->SetViewTargetWithBlend(Snake1);
		}
	}
}
ANewSnakePawn* ASnakeGameMode::SpawnSnakeForPlayer(int32 PlayerIndex, FVector SpawnLocation) {
	if (!SnakeBlueprintClass) {
		UE_LOG(LogTemp, Error, TEXT("SnakeBlueprintClass not assigned in GameMode BP!"));
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Use SnakeBlueprintClass instead of ANewSnakePawn::StaticClass()
	return GetWorld()->SpawnActor<ANewSnakePawn>(SnakeBlueprintClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
}
