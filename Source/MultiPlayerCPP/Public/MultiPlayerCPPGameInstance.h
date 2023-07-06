// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OnlineSubSystem.h"
#include "OnlineSessionSettings.h"
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiPlayerCPPGameInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType) struct FServerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)	FString ServerName;
	UPROPERTY(BlueprintReadOnly)	int32 MaxPlayersAmount;
	UPROPERTY(BlueprintReadOnly)	int32 CurrentPlayersAmount;
	UPROPERTY(BlueprintReadOnly)int32 SessionIndex;
	UPROPERTY(BlueprintReadOnly)bool bIsLan;
	UPROPERTY(BlueprintReadOnly)int32 Ping;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerFindDelegate, FServerInfo, ServerListDelegate);

UCLASS()
class MULTIPLAYERCPP_API UMultiPlayerCPPGameInstance : public UGameInstance
{
	GENERATED_BODY()
public: UMultiPlayerCPPGameInstance();

protected:
	FName DefaultSessionName= "Default Session Name";
	UPROPERTY(BlueprintAssignable) FServerFindDelegate ServerListDelegate;
	virtual void OnCreateSessionComplete(FName SessionName, bool Succeded);
	virtual void OnFindSessionsComplete(bool Succeded);
	virtual void OnDestroySessionComplete(FName SessionName, bool Succeded);
	virtual void OnJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type ResultType);
	virtual	void Init() override;
	IOnlineSessionPtr SessionInterface;
	UFUNCTION (BlueprintCallable) void CreateServer(FString ServerName, FString HostName);
	UFUNCTION(BlueprintCallable) void JoinServer(int32 SessionIndex);
	UFUNCTION(BlueprintCallable) void FindServers();
	UFUNCTION(BlueprintCallable) void LeaveServer();
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};
