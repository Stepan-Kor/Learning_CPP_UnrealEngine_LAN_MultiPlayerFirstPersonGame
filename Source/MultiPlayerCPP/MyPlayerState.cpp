// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "MultiPlayerCPPCharacter.h"


AMyPlayerState::AMyPlayerState()
{
	bReplicates = true;
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AMyPlayerState, StateHits);
}

void AMyPlayerState::OnRep_HitsChanged()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("Player State Hits changed %i"), StateHits));
}

void AMyPlayerState::PlayerHited(AMultiPlayerCPPCharacter* Player)
{
	if(HasAuthority()){
	StateHits++;
	GEngine->AddOnScreenDebugMessage(-1,2,FColor::Green,FString::Printf(TEXT("Player State Hit warned %i"),StateHits));
	}
}
