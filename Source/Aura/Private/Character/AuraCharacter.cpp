// Copyright Marina Gloc


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "Aura_PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Aura_PlayerController.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.);
	GetCharacterMovement()->bConstrainToPlane=true;
	GetCharacterMovement()-> bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Init Ability Actor info for the server
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//Init Ability Actor info for the client
	InitAbilityActorInfo();
	
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAura_PlayerState* AuraPlayerState = GetPlayerState<AAura_PlayerState>();
	check(AuraPlayerState);
	AuraPlayerState-> GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	AbilitySystemComponent= AuraPlayerState-> GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if(AAura_PlayerController* AuraPlayerController = Cast<AAura_PlayerController>(GetController()))
	{
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	
}
