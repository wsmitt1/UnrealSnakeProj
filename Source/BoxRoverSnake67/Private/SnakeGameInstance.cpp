// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "CoreMinimal.h"

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
			if (LoseScreenClass)
			{
				UUserWidget* LoseWidget = CreateWidget<UUserWidget>(GetWorld(), LoseScreenClass);
				if (LoseWidget)
				{
					LoseWidget->AddToViewport();

					// Give control to the mouse, get player 0's controller, and set the input mode to UI only so they can click the "Play Again" button
					APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
					if (PC)
					{
						PC->SetShowMouseCursor(true);
						FInputModeUIOnly InputMode;
						InputMode.SetWidgetToFocus(LoseWidget->TakeWidget());
						PC->SetInputMode(InputMode);
					}
				}
			}
			break;
	}
}