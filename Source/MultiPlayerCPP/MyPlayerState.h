// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"
class AMultiPlayerCPPCharacter;

/**
 * 
 */
UCLASS()
class MULTIPLAYERCPP_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
		UPROPERTY(ReplicatedUsing = OnRep_HitsChanged) uint32 StateHits;
protected:
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& LifetimeReplicatedProps)const override;

	public:
		AMyPlayerState();

	virtual	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
		UFUNCTION() void OnRep_HitsChanged();
		void PlayerHited(AMultiPlayerCPPCharacter* Player);
};
