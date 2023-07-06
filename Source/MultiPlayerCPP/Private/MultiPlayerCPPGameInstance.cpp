// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MultiPlayerCPPGameInstance.h"



UMultiPlayerCPPGameInstance::UMultiPlayerCPPGameInstance() {}



void UMultiPlayerCPPGameInstance::Init() {
	Super::Init();
	//GEngine->AddOnScreenDebugMessage(0, 12, FColor::Orange, FString ("custom game instance started"), true);

	if (IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get()) {

		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface) {
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiPlayerCPPGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiPlayerCPPGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiPlayerCPPGameInstance::OnJoinSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiPlayerCPPGameInstance::OnDestroySessionComplete);
		}
	}
}

void UMultiPlayerCPPGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeded)
{
	UE_LOG ( LogTemp, Warning, TEXT("Session created %d"), Succeded);
	if (Succeded) GetWorld()->ServerTravel("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap?listen");
}

void UMultiPlayerCPPGameInstance::OnFindSessionsComplete(bool Succeded)
{
	UE_LOG(LogTemp,Warning,TEXT("Searching for sessions complete"));
	if (Succeded) {
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		int32 Index = 0;
		for (FOnlineSessionSearchResult Result : SearchResults) {

			if (Result.IsValid()) {
				FServerInfo ServerInfo;
				FString ServerName = "Empty Server Name";
				FString HostName = "Empty Host Name";
				Result.Session.SessionSettings.Get(FName ("SERVER_NAME_KEY") , ServerName);
				ServerInfo.ServerName = ServerName;
				ServerInfo.MaxPlayersAmount = Result.Session.SessionSettings.NumPublicConnections;
				ServerInfo.CurrentPlayersAmount = ServerInfo.MaxPlayersAmount - Result.Session.NumOpenPublicConnections;
				ServerInfo.SessionIndex = Index;
				ServerInfo.bIsLan = Result.Session.SessionSettings.bIsLANMatch;
				ServerInfo.Ping = Result.PingInMs;
				ServerListDelegate.Broadcast(ServerInfo);
			}
			Index++;
		}
		UE_LOG(LogTemp, Warning, TEXT("SearchResults, Servers Count: %d"), SearchResults.Num());
		if (SearchResults.Num()) { 
			//SessionInterface->JoinSession(0, ("Test Session CPPMultiplayer"), SearchResults[0]);
		}
	}
}

void UMultiPlayerCPPGameInstance::OnDestroySessionComplete(FName SessionName, bool Succeded)
{
	UE_LOG(LogTemp,Warning,TEXT("Session destroyed %d"), Succeded);
}

void UMultiPlayerCPPGameInstance::OnJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type ResultType)
{
	if(SessionInterface){
		if (ResultType == EOnJoinSessionCompleteResult::Success) {
			UE_LOG(LogTemp, Warning, TEXT("Joining session %s complete"), *Name.ToString());
			if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
				FString TravelAdress="";
				SessionInterface->GetResolvedConnectString(Name, TravelAdress);
				if(TravelAdress!="")		PlayerController->ClientTravel(TravelAdress, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	else {
		UE_LOG(LogTemp,Warning,TEXT("Join Session %s failed"), *Name.ToString());
	}
}

void UMultiPlayerCPPGameInstance::CreateServer(FString ServerName, FString HostName)
{
	if (SessionInterface) {
		UE_LOG(LogTemp, Warning, TEXT("Creating session . . ."));
		FOnlineSessionSettings SessionSettings;
		//SessionSettings.Settings.Name
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bIsDedicated = false;
		SessionSettings.bIsLANMatch = true;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.NumPublicConnections = 5;
		SessionSettings.Set(FName("SERVER_NAME_KEY"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(FName("SERVER_HOSTNAME_KEY"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionInterface->CreateSession(0, DefaultSessionName, SessionSettings);
		}
	}

void UMultiPlayerCPPGameInstance::JoinServer(int32 SessionIndex)
{
	if (SessionSearch.IsValid()) {
	TArray <FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
	if (SearchResults.Num()>SessionIndex)		SessionInterface->JoinSession(0, DefaultSessionName, SearchResults[SessionIndex]);
	else UE_LOG(LogTemp, Warning, TEXT("No solid servers found"));
	}
	else UE_LOG(LogTemp, Warning, TEXT("No search been done"));
}

void UMultiPlayerCPPGameInstance::FindServers()
{
	UE_LOG(LogTemp, Warning, TEXT("Searching for sessions . . ."));
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery=true;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMultiPlayerCPPGameInstance::LeaveServer()
{
	if(SessionInterface.IsValid())SessionInterface->DestroySession(DefaultSessionName);
}
