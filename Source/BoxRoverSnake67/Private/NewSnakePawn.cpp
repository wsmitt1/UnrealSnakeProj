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
#include "SnakeGameInstance.h"

// Sets default values

ANewSnakePawn::ANewSnakePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//AutoPossessPlayer = EAutoReceiveInput::Player0; // Possesses pawn manually in GameMode instead now.

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SnakeRoot"));
	RootComponent = CollisionSphere;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1000.0f; // Distance from the pawn
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

	// Get speed based on GameInstance difficulty variable
	USnakeGameInstance* GI = Cast<USnakeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		if (DifficultyToSpeed.Contains(GI->Difficulty))
		{
			Speed = DifficultyToSpeed[GI->Difficulty];
		}
		else
		{
			Speed = 100.0f; // Default speed if difficulty not found
		}
	}


	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANewSnakePawn::OnOverlapBegin);
	float TileSize = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()))->TileSize;
	// Start at our current grid position
	LogicPrevPosition = GetActorLocation();
	LogicTargetPosition = LogicPrevPosition;
	MovementInterpolation = 1.0f;


	SegmentLogicPositions.Add(LogicPrevPosition);
	SegmentPrevLogicPositions.Add(LogicPrevPosition);
}

void ANewSnakePawn::SelectNewTargetTile()
{

	SegmentPrevLogicPositions = SegmentLogicPositions;


	FVector Direction3D = FVector(CurrentDirection.Y, CurrentDirection.X, 0.0f); // Swapped Y and X because i dont fucking know man it just works
	SegmentLogicPositions[0] += (Direction3D * 100.0f);


	for (int32 i = 1; i < SegmentLogicPositions.Num(); i++)
	{
		SegmentLogicPositions[i] = SegmentPrevLogicPositions[i - 1];
	}


	MovementInterpolation = 0.0f;
	LogicPrevPosition = SegmentPrevLogicPositions[0];
	LogicTargetPosition = SegmentLogicPositions[0];
}

void ANewSnakePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (CurrentDirection.IsZero()) return;
	MovementInterpolation += (Speed / 100.0f) * DeltaTime;


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
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
			{
				if (LocalPlayer->GetControllerId() == 0) { // Player 1
					if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
					{
						EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Started, this, &ANewSnakePawn::Turn);
						UE_LOG(LogTemp, Warning, TEXT("Bound TurnAction to Player 0"));
					}
				}
				else { // Player 2 or more
					if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
					{
						EnhancedInputComponent->BindAction(TurnActionPlayer1, ETriggerEvent::Started, this, &ANewSnakePawn::Turn);
						UE_LOG(LogTemp, Warning, TEXT("Bound TurnActionPlayer1 to Player 1"));
					}
				}
			}
		}
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
			UE_LOG(LogTemp, Warning, TEXT("Collided with Wall or Tail"));
			ShowLoseScreen();
		}
		else if (OtherActor->IsA(AFruit::StaticClass()))
		{
			EatFruit(OtherActor);
		}
		else if (OtherActor->IsA(TailClass) or (OtherActor->IsA(ANewSnakePawn::StaticClass()) && OtherActor != this))
		{
			// 2. Is it NOT the first segment (the neck)?
			// We check index 0 because that's the one usually "touching" the head
			if (TailSegments.Num() > 0 && OtherActor != TailSegments[0])
			{
				ShowLoseScreen();
			}
		}
	}
}

void ANewSnakePawn::ShowLoseScreen()
{
	Speed = 0.0f;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Crash, GetActorLocation(), 1.5f, 1.0f, 0.2f);
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

		NewSegment->Tags.Add(FName("TailSegment"));

		// Add logic tracking for the new segment
		// It starts exactly where the last segment currently is
		FVector LastPos = SegmentPrevLogicPositions.Last();
		SegmentLogicPositions.Add(LastPos);
		SegmentPrevLogicPositions.Add(LastPos);


	}
}
void ANewSnakePawn::Turn(const FInputActionValue& Value)
{

    const FVector2D Input2D = Value.Get<FVector2D>();


	if (Input2D.IsZero() == false) {
		if (Input2D.Length() <= 1.0f) { // Ignore diagonals: may be changed later 
			CurrentDirection = InvertedControls ? -Input2D.GetSafeNormal() : Input2D.GetSafeNormal();

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

	if (Cast<AFruit>(FruitActor)->InvertUserControls)
	{
		InvertedControls = !InvertedControls;
	}

	FruitActor->Destroy();

	// play eat sound
	if (Eat)
	{
		// Use GetWorld() as the WorldContextObject to match the PlaySoundAtLocation overload
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Eat, GetActorLocation());
	}
	
	AFruitSpawner* Spawner = Cast<AFruitSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AFruitSpawner::StaticClass()));
	if (Spawner)
	{
		Spawner->SpawnNewFruit();
	}
	
	SpawnTailSegment();
}

void ANewSnakePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APlayerController* PC = Cast<APlayerController>(NewController);
	if (PC)
	{
		// 1. Set Input Mode
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;

		// 2. Add Enhanced Input Mapping using the LocalPlayer's subsystem and log controller id
		ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
		if (LocalPlayer)
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				// Make sure 'InputMapping' is assigned in your BP!


				// Log using the LocalPlayer's controller id (valid method on ULocalPlayer)
				int32 ControllerId = LocalPlayer->GetControllerId();
				if (ControllerId == 0) {
					UE_LOG(LogTemp, Warning, TEXT("Adding WASD Mapping Context for Player %d"), ControllerId);
					Subsystem->AddMappingContext(InputMappingContext_Keyboard, 0);
				}
				else if (ControllerId == 1) {

					UE_LOG(LogTemp, Warning, TEXT("Adding Arrow Keys Mapping Context for Player %d"), ControllerId);
					Subsystem->AddMappingContext(InputMappingContext_Gamepad, 1);
				}
				UE_LOG(LogTemp, Warning, TEXT("Mapping Context Added for Player %d"), ControllerId);
			}
		}
	}
}

