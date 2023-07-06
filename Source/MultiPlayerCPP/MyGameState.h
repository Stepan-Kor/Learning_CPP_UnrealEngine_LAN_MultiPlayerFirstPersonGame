// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/TextRenderActor.h"
#include "MyGameState.generated.h"
class ATextRenderActor;
/**
 * 
 */
UCLASS()
class MULTIPLAYERCPP_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
		AMyGameState();
		UFUNCTION(Server, Unreliable) void SetOwnership();
		void SetOwnership_Implementation();
		virtual void BeginPlay() override;
		UFUNCTION() void PlayerHited();
		UFUNCTION() void HitsChanged();
		ATextRenderActor* Scenetext;
		UPROPERTY(BlueprintReadOnly,ReplicatedUsing =HitsChanged)int32 PlayersHits=0;
		UFUNCTION() void ChangeHitsText();
};
