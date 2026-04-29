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
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h" // Added include for USpringArmComponent
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"



// Provide an IntelliSense-only fallback for GENERATED_BODY() to avoid E0077
// Visual Studio's IntelliSense defines __INTELLISENSE__; only in that case define a minimal macro.
// This will not affect actual compilation by UnrealBuildTool.


#include "NewSnakePawn.generated.h"

class UStaticMeshComponent;

class UCameraComponent;


UCLASS()
class BOXROVERSNAKE67_API ANewSnakePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties

	void PossessedBy(AController* NewController) override;

	ANewSnakePawn();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Turn(const FInputActionValue& Value);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "¨nake", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> VisibleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TurnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TurnActionPlayer1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SnakeConfig", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LoseScreenClass;

	UPROPERTY(EditAnywhere, Category = "Snake")
    TSubclassOf<AActor> TailClass;

    // The list of all spawned tail segments
    UPROPERTY()
    TArray<AActor*> TailSegments;

    // Stores the sequence of logic positions for segments to follow
    TArray<FVector> TailPathHistory;


	UFUNCTION()
	void ShowLoseScreen();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EatFruit(AActor* FruitActor);


	void SpawnTailSegment();
	void UpdateTailPositions();

	UCameraComponent* Camera;

	FVector2D CurrentTargetedPosition;
	FVector2D RegisteredPosition;

	// Index 0 = Head, Index 1 = Tail 0, Index 2 = Tail 1...
	TArray<FVector> SegmentLogicPositions;

	// We need to know where they were in the previous step to Lerp correctly
	TArray<FVector> SegmentPrevLogicPositions;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* IMC_WASD;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* IMC_Arrows;

private:
	FVector LogicPrevPosition;   // The center of the tile we just left
	FVector LogicTargetPosition; // The center of the tile we are moving toward
	float MovementInterpolation; // Progress from 0.0 to 1.0

	void SelectNewTargetTile();

	float snakeTileSize;

	FVector QueuedDirection;
	FVector CurrentDirection;
	FVector2D LastInputVector;
};
