// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameInstance.h"

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
	// Handle state change logic here, such as loading different levels or showing UI
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