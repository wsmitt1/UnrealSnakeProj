// Fill out your copyright notice in the Description page of Project Settings.

#include "NewSnakePawn.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
// Sets default values
ANewSnakePawn::ANewSnakePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Properties


	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SnakeRoot"));
	UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
	// Attach our camera and visible object to our root component. Offset and rotate the camera.
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));
	Camera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	VisibleComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANewSnakePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANewSnakePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANewSnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Started, this, &ANewSnakePawn::Turn);
	}
}

void ANewSnakePawn::Turn(const FInputActionValue& Value)
{
	// Add movement in that direction
	
	AddActorLocalOffset(FVector(Value.Get<float>(), 0.0f, 0.0f), true);
}

UPROPERTY(EditAnywhere)
USceneComponent* VisibleComponent;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
TObjectPtr<UInputMappingContext> InputMapping;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
TObjectPtr<UInputAction> TurnAction;