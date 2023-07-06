// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERCPP_API AMultiPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMultiPlayerController();
	UFUNCTION(Server, Unreliable) void SetOwnership();
	void SetOwnership_Implementation();

	virtual void BeginPlay() override;
};
