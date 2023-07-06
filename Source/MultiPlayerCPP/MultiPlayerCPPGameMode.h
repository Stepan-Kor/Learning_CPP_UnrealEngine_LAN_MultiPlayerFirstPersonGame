// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiPlayerCPPGameMode.generated.h"

UCLASS(minimalapi)
class AMultiPlayerCPPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiPlayerCPPGameMode();
	UFUNCTION() void PlayerHited();
};



