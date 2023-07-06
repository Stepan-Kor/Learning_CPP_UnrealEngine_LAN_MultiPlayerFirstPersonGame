// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "InteractionInterface.h"
#include "Net/UnrealNetwork.h"
#include "Engine/TextRenderActor.h"
#include "Components/TextRenderComponent.h"

AMyGameState::AMyGameState()
{
	bReplicates = true;
}
	

void AMyGameState::GetLifetimeReplicatedProps(TArray <FLifetimeProperty> &OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME (AMyGameState, PlayersHits);
}


void AMyGameState::SetOwnership_Implementation()
{
	if (this->HasAuthority()) {
	TArray <AActor*> TempActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UInteractionInterface::StaticClass(), TempActors);
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "SERVER Colected actors with interface");
	}
}

void AMyGameState::BeginPlay()
{
	//bReplicates = true;
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::MakeRandomColor(), "GameState event beginplay");
	//Super::BeginPlay();
	//SetOwnership();
	TArray <AActor*> TempActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ATextRenderActor::StaticClass(), "TotalHits", TempActors);
	if (TempActors.IsValidIndex(0)) {
		Scenetext = Cast<ATextRenderActor>(TempActors[0]);
		if(Scenetext)Scenetext->GetTextRender()->SetText("0 Hits");

	}
}

void AMyGameState::PlayerHited()
{
	PlayersHits++;
	if(HasAuthority())ChangeHitsText();
	//GEngine->AddOnScreenDebugMessage(-1,2,FColor::Emerald, FString::Printf(TEXT("Game State hit registered %i"), PlayersHits));
}

void AMyGameState::HitsChanged()
{
	//PlayersHits++;
	ChangeHitsText();
}

void AMyGameState::ChangeHitsText()
{
	if (Scenetext) {
		FString String = FString::FromInt(PlayersHits);
		String.Append(" hits was made");
		Scenetext->GetTextRender()->SetText(String);
		//UTextRenderComponent* TextRend =Scenetext->GetTextRender();
	}
}
