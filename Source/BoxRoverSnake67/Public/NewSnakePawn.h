// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Provide an IntelliSense-only fallback for GENERATED_BODY() to avoid E0077
// Visual Studio's IntelliSense defines __INTELLISENSE__; only in that case define a minimal macro.
// This will not affect actual compilation by UnrealBuildTool.
#ifdef __INTELLISENSE__
#ifndef GENERATED_BODY
#define GENERATED_BODY() \
public:
#endif
#endif

#include "NewSnakePawn.generated.h"

class UCameraComponent;

UCLASS()
class BOXROVERSNAKE67_API ANewSnakePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANewSnakePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USceneComponent* VisibleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TurnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SnakeConfig", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UCameraComponent* Camera;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Turn(const FInputActionValue& Value);



	FVector2D CurrentDirection;
};
