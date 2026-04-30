// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameInstance.h"
#include "SnakePlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


USnakeGameInstance::USnakeGameInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    CurrentGameState = EGameState::MainMenu;
    Difficulty = "Normal";
    bIsMultiplayer = false;
}
void USnakeGameInstance::OnStateChanged(EGameState NewState) {
	switch (NewState) {
		case EGameState::MainMenu:
			// Load main menu level or show main menu UI
			break;
		case EGameState::Playing:
			// Load gameplay level or hide main menu UI
			break;
        case EGameState::Outro:
            // Check GetWorld() first!
            if (GetWorld() && LoseScreenClass)
            {
                // Player 1
                APlayerState* PS1 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
                ASnakePlayerState* SnakePS1 = Cast<ASnakePlayerState>(PS1);
                if (SnakePS1) {
                    FinalScore_P1 = SnakePS1->PlayerScore;
                }

                // Player 2
                if (bIsMultiplayer) {
                    APlayerState* PS2 = UGameplayStatics::GetPlayerState(GetWorld(), 1);
                    ASnakePlayerState* SnakePS2 = Cast<ASnakePlayerState>(PS2);
                    if (SnakePS2) {
                        FinalScore_P2 = SnakePS2->PlayerScore;
                    }
                }
                    UUserWidget* LoseWidget = CreateWidget<UUserWidget>(GetWorld(), LoseScreenClass);
                    if (LoseWidget)
                    {
                        LoseWidget->AddToViewport();

                        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
                        if (PC)
                        {
                            PC->bShowMouseCursor = true; // Use the property directly or the setter
                            FInputModeUIOnly InputMode;
                            InputMode.SetWidgetToFocus(LoseWidget->TakeWidget());
                            PC->SetInputMode(InputMode);
                        }
                    }
                }
                break;
            
	}
}