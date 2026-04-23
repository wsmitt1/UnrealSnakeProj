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
#include "EnhancedInputSubsystems.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values

ANewSnakePawn::ANewSnakePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SnakeRoot"));
	RootComponent = CollisionSphere;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f; // Distance from the pawn
	SpringArm->bUsePawnControlRotation = false; // Don't rotate the arm based on the controller
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f)); // Look down at the pawn
	SpringArm->bDoCollisionTest = false; // Don't let the arm adjust based on collisions

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
	VisibleComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANewSnakePawn::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
	
}

// Called every frame
void ANewSnakePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Scale movement by Speed and DeltaTime for smooth motion
	FVector Movement = FVector(CurrentDirection.Y, CurrentDirection.X, 0.0f) * Speed * DeltaTime; // i dont know what kind of black magic this is but it works so im not gonna question it

	AddActorWorldOffset(Movement, true);
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
    // Extract the Vector2 input from the action value
    const FVector2D Input2D = Value.Get<FVector2D>();

    // Update the pawn's current input direction (store normalized direction if desired)
    // If you want raw input keep Input2D; to keep only direction use Input2D.GetSafeNormal()
	if (Input2D.IsZero() == false) {
		if (Input2D.Length() < 1.0f) { // Ignore diagonals: may be changed later 
			CurrentDirection = Input2D.GetSafeNormal(); // requires CurrentDirection declared in header

		}
	}

    //AddActorLocalOffset(FVector(CurrentDirection.X, 0.0f, 0.0f), true);
}
