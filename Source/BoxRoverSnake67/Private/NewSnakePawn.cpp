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
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Custom class includes
#include "Fruit.h"
#include "FruitSpawner.h"
#include "SnakePlayerState.h"
#include "GridManager.h"

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
	SpringArm->TargetArmLength = 700.0f; // Distance from the pawn
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

		PC->SetShowMouseCursor(false);
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANewSnakePawn::OnOverlapBegin);
	float TileSize = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()))->TileSize;
	// Start at our current grid position
	LogicPrevPosition = GetActorLocation();
	LogicTargetPosition = LogicPrevPosition;
	MovementInterpolation = 1.0f; // Start "arrived"

	// Inside BeginPlay
	SegmentLogicPositions.Add(LogicPrevPosition);
	SegmentPrevLogicPositions.Add(LogicPrevPosition);
}

void ANewSnakePawn::SelectNewTargetTile()
{
	// 1. Update "Previous" record for the whole snake
	SegmentPrevLogicPositions = SegmentLogicPositions;

	// 2. Move the Head Logic (Index 0)
	FVector Direction3D = FVector(CurrentDirection.Y, CurrentDirection.X, 0.0f); // Swapped Y and X because i dont fucking know man it just works
	SegmentLogicPositions[0] += (Direction3D * 100.0f);

	// 3. Move the Tail Logic
	// Each segment moves to where the segment in front of it WAS
	for (int32 i = 1; i < SegmentLogicPositions.Num(); i++)
	{
		SegmentLogicPositions[i] = SegmentPrevLogicPositions[i - 1];
	}

	// Reset interpolation for the smooth slide
	MovementInterpolation = 0.0f;
	LogicPrevPosition = SegmentPrevLogicPositions[0];
	LogicTargetPosition = SegmentLogicPositions[0];
}

void ANewSnakePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Don't move if there is no direction set
	if (CurrentDirection.IsZero()) return;
	MovementInterpolation += (Speed / 100.0f) * DeltaTime;

	// 2. If we've reached or passed the target tile, find the next one
	if (MovementInterpolation >= 1.0f)
	{
		SelectNewTargetTile();
	}

	FVector NewSmoothLocation = FMath::Lerp(LogicPrevPosition, LogicTargetPosition, MovementInterpolation);

	SetActorLocation(NewSmoothLocation, true);

	for (int32 i = 0; i < TailSegments.Num(); i++)
	{
		// TailSegments[0] corresponds to SegmentLogicPositions[1]
		int32 LogicIndex = i + 1;

		if (TailSegments.IsValidIndex(i) && SegmentLogicPositions.IsValidIndex(LogicIndex))
		{
			FVector NewSegLoc = FMath::Lerp(SegmentPrevLogicPositions[LogicIndex], SegmentLogicPositions[LogicIndex], MovementInterpolation);
			TailSegments[i]->SetActorLocation(NewSegLoc);
		}
	}
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

void ANewSnakePawn::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
	if (OtherActor && OtherActor != this)
	{
		if (OtherActor->ActorHasTag(FName("Wall"))) {
			UE_LOG(LogTemp, Warning, TEXT("Collided with Wall"));
			ShowLoseScreen();
		}
		else if (OtherActor->IsA(AFruit::StaticClass()))
		{
			EatFruit(OtherActor);
		}
	}
}

void ANewSnakePawn::ShowLoseScreen()
{
	Speed = 0.0f;

	if (LoseScreenClass)
	{
		UUserWidget* LoseWidget = CreateWidget<UUserWidget>(GetWorld(), LoseScreenClass);
		if (LoseWidget)
		{
			LoseWidget->AddToViewport();

			// Give control to the mouse
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				PC->SetShowMouseCursor(true);
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(LoseWidget->TakeWidget());
				PC->SetInputMode(InputMode);
			}
		}
	}
}

void ANewSnakePawn::SpawnTailSegment()
{
	// Spawn the actor at the very last segment's position (or head if it's the first)
	FVector SpawnLoc = (TailSegments.Num() == 0) ? GetActorLocation() : TailSegments.Last()->GetActorLocation();

	AActor* NewSegment = GetWorld()->SpawnActor<AActor>(TailClass, SpawnLoc, FRotator::ZeroRotator);
	if (NewSegment)
	{
		TailSegments.Add(NewSegment);

		// Add logic tracking for the new segment
		// It starts exactly where the last segment currently is
		FVector LastPos = SegmentLogicPositions.Last();
		SegmentLogicPositions.Add(LastPos);
		SegmentPrevLogicPositions.Add(LastPos);
	}
}
void ANewSnakePawn::Turn(const FInputActionValue& Value)
{

    const FVector2D Input2D = Value.Get<FVector2D>();


	if (Input2D.IsZero() == false) {
		if (Input2D.Length() <= 1.0f) { // Ignore diagonals: may be changed later 
			CurrentDirection = Input2D.GetSafeNormal(); 

		}
	}


}
void ANewSnakePawn::EatFruit(AActor* FruitActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Ate Fruit!"));

	ASnakePlayerState* PS = Cast<ASnakePlayerState>(GetPlayerState());

	if (PS)
	{
		PS->AddScore(Cast<AFruit>(FruitActor)->ScoreValue);
	}

	FruitActor->Destroy();
	
	AFruitSpawner* Spawner = Cast<AFruitSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AFruitSpawner::StaticClass()));
	if (Spawner)
	{
		Spawner->SpawnNewFruit();
	}
	
	SpawnTailSegment();
}

