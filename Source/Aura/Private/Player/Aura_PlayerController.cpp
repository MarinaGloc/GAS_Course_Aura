// Copyright Marina Gloc


#include "Player/Aura_PlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAura_PlayerController::AAura_PlayerController()
{
	bReplicates = true;
}

void AAura_PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAura_PlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if(!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	/*
	 * Line Trace from cursor. There are several scenarios:
	 * A. LastActor is NULL && ThisActor is NULL
	 *		- Do Nothing
	 * B. LastActor is NULL && ThisActor is VALID
	 *		- Highlight ThisActor
	 * C. LastActor is VALID && ThisActor is NULL
	 *		- UnHighlight LastActor
	 * D. LastActor is VALID && ThisActor is valid != LastActor
	 *		- UnHighlight LastActor, and Highlight ThisActor
	 * E. Both actors are valid, and they are the same actor
	 *		- Do nothing 
	 */

	if (LastActor == nullptr)
	{
		if(ThisActor != nullptr)
		{
			//case B
			ThisActor-> HighlightActor();
		}
		else
		{
			//Case A - both are null, do nothing
		}
	}
	else //LastActor is Valid
	{
		if(ThisActor == nullptr)
		{
			//Case C
			LastActor-> UnHighlightActor();
		}
		else // Both actor are valid
		{
			if(LastActor!=ThisActor)
			{
				//Case D
				LastActor-> UnHighlightActor();
				ThisActor-> HighlightActor();
			}
			else
			{
				//Case E - Do nothing
			}
		}
	}
}


void AAura_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAura_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAura_PlayerController::Move);
}

void AAura_PlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2d InputAxisVector = InputActionValue.Get<FVector2d>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);	
	}
	
}

