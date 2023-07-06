// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiPlayerCPPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"



// Sets default values
ADoor::ADoor()
{
	bReplicates = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal_Mesh_Door"));
	RootComponent = Mesh;
	ConstructorHelpers::FObjectFinder<UAnimationAsset> OpenAnimAsset(TEXT("AnimSequence'/Game/Door/A_Door_Open.A_Door_Open'"));
	ConstructorHelpers::FObjectFinder<UAnimationAsset> CloseAnimAsset(TEXT("AnimSequence'/Game/Door/A_Door_Close.A_Door_Close'"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletAsset(TEXT("SkeletalMesh'/Game/Door/Door.Door'"));
	if (SkeletAsset.Succeeded())Mesh->SetSkeletalMesh(SkeletAsset.Object);
	if (CloseAnimAsset.Succeeded())AnimClose=CloseAnimAsset.Object;
	if (OpenAnimAsset.Succeeded())AnimOpen = OpenAnimAsset.Object;
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	//SetOwner(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetOwner());
	//if (HasAuthority())SentOwner(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//SetOwner(Cast<AActor>(UGameplayStatics::GetPlayerController(GetWorld(),0)->GetPlayerState<APlayerState>()));
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor(212,22,72,255), FString::Printf(TEXT("Owner of %s seted to %s"), *this->GetName(), *this->GetOwner()->GetName()));

	if (HasAuthority() && false) {
		//SentOwner(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		//SetOwner(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		
		FTimerHandle UnusedHandle;
		GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &ADoor::SetAuthority, 5, false);
	}
	//if (IsValid(GetOwner()))GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, FString::Printf(TEXT("Owner of %s seted to %s"), *this->GetName(), *this->GetOwner()->GetName()));
	//NameOftheEvent.AddDynamic
	NameOfTheEventsVariable.AddDynamic(this, &ADoor::EventBroadcastFunction);
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, bOpen);
}

void ADoor::DoorToggle()
{

	//bOpen = !bOpen;
	//if(HasAuthority())	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "has autority");
	//else	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "has NO autority");
	//Server_DoorToggle();
	//Multi_DoorToggle();
	//if (IsValid(GetOwner())) {		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("Owner is  %s"), *this->GetOwner()->GetName()));	}
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ADoor::RealToggleDoor()
{
	if (IsValid(Mesh)) {
		Mesh->PlayAnimation(bOpen ? AnimOpen : AnimClose, false);
		//if (!bOpen)Mesh->PlayAnimation(AnimClose, false);
		//else Mesh->PlayAnimation(AnimOpen, false);
		//bOpen = !bOpen;
	}
}

void ADoor::EventBroadcastFunction(AMultiPlayerCPPCharacter* Character)
{
	if(Character)GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("%s's event called by %s"), *this->GetName(), *Character->GetName()));
	Server_DoorToggle();
}

bool ADoor::Interaction_Implementation(AMultiPlayerCPPCharacter* Character)
{
	//NameOfTheEventsVariable.Broadcast(Character);
	//if (IsValid(GetOwner()))GEngine->AddOnScreenDebugMessage(1, 2, FColor::Green, FString::Printf(TEXT("%s owned by %s"), *this->GetName(), *GetOwner()->GetName()));
	//else GEngine->AddOnScreenDebugMessage(1, 2, FColor::Green, FString::Printf(TEXT("%s dosnt have owner"), *this->GetName()));
	//NameOfTheEventsVariable.Broadcast(Character);
	//Multi_DoorToggle();
	//Character->GetName();
	//DoorToggle();
	//Server_DoorToggle();
	if (HasAuthority()) {
		bOpen = !bOpen;
		RealToggleDoor();
	}
	return true;
}

void ADoor::SetAuthority()
{ 
	SetOwner(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//UGameplayStatics::GetPlayerCharacter(GetWorld(), 1) ? SetOwner(UGameplayStatics::GetPlayerCharacter(GetWorld(), 1)->GetPlayerState());
	
	if (IsValid(GetOwner()))GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Server set Owner of %s to %s"), *this->GetName(), *this->GetOwner()->GetName()));
}

void ADoor::OnRep_DoorToggle()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "bOpen changed");
	
	RealToggleDoor();
}

bool ADoor::Server_DoorToggle_Validate()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("%s validated"), *this->GetName()));
	return true;
}

void ADoor::Server_DoorToggle_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Purple, FString::Printf(TEXT("Server door function %s"), *this->GetName()));
	bOpen = !bOpen;
	Multi_DoorToggle();
}

void ADoor::SentOwner_Implementation(AActor* DoorOwner)
{
	if (IsValid(DoorOwner)) {
		SetOwner(DoorOwner);
		//FString TempString = DoorOwner->GetName(); 
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("%s get owner %s"), *this->GetName(), *DoorOwner->GetName()));
	}
}

bool ADoor::Multi_DoorToggle_Validate()
{
	return true;
}

void ADoor::Multi_DoorToggle_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "Multi call hapened");
	RealToggleDoor();
}