// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "SnakeGameInstance.generated.h"

UENUM()
enum class EGameState : uint8 {
	MainMenu,
	Playing,
	Outro
};
/**
 * 
 */


UCLASS()
class BOXROVERSNAKE67_API USnakeGameInstance : public UGameInstance {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "Game Settings")
	bool bIsMultiplayer = false;

	UPROPERTY(BlueprintReadWrite, Category = "Game Settings")
	FString Difficulty = "Normal";

	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void SetGameState(EGameState NewState) {
		if (CurrentGameState != NewState) {
			CurrentGameState = NewState;
			OnStateChanged(NewState);
		}
	}

protected:
	UPROPERTY(BlueprintReadOnly, Category = "StateMachine")
	EGameState CurrentGameState = EGameState::MainMenu;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LoseScreenClass;

private:
	void OnStateChanged(EGameState NewState);  // Handle state change logic here, such as loading different levels or showing UI
};
