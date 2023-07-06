// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "InteractionInterface.h"
//#include "Engine/World.h"

AMultiPlayerController::AMultiPlayerController()
{
	bReplicates = true;
}

void AMultiPlayerController::SetOwnership_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, "SERVER Controller colectes actors with interface");
	TArray <AActor*> TempActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(),UInteractionInterface::StaticClass(),TempActors);
	for (AActor* ArrayActor : TempActors) {
		if(IsValid(ArrayActor))ArrayActor->SetOwner(UGameplayStatics::GetPlayerController(GetWorld(), 2));
	}
	
}

void AMultiPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//if(GetInputIndex()!=0)
	//if (!HasAuthority()) {
	//	SetOwnership();
	//	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, FString::Printf(TEXT("Controller has autority %s"), *this->GetName()));
	//}
}
