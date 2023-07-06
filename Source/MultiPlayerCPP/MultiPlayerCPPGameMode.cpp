// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiPlayerCPPGameMode.h"
#include "MultiPlayerCPPHUD.h"
#include "MultiPlayerCPPCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MyGameState.h"

AMultiPlayerCPPGameMode::AMultiPlayerCPPGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	static ConstructorHelpers::FClassFinder<AMyGameState> GameStateClassFinder(TEXT("Class'/Script/MultiPlayerCPP.MyGameState'"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	GameStateClass = GameStateClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMultiPlayerCPPHUD::StaticClass();
}

void AMultiPlayerCPPGameMode::PlayerHited()
{
	if (AMyGameState* GS =GetGameState<AMyGameState>()) {
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Purple, "Game mode hit hapen");
		GS->PlayerHited();
	}
	//GetGameState()
}
