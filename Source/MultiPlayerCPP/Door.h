// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "Door.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FNameOfTheEvent, AMultiPlayerCPPCharacter* ,Character);
UCLASS()
class MULTIPLAYERCPP_API ADoor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();
	UPROPERTY(EditDefaultsOnly) class USkeletalMeshComponent* Mesh;
	UPROPERTY( ReplicatedUsing= OnRep_DoorToggle,VisibleAnywhere, BlueprintReadOnly) bool bOpen=false;
	UPROPERTY(Replicated) bool bTest;
	UPROPERTY(EditAnywhere) UAnimationAsset* AnimOpen;
	UPROPERTY(EditAnywhere)	UAnimationAsset* AnimClose;
	UPROPERTY(BlueprintAssignable, BlueprintCallable) FNameOfTheEvent NameOfTheEventsVariable;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& LifetimeReplicatedProps)const override;

	//UFUNCTION()void testF();
	UFUNCTION()void OnRep_DoorToggle();
	UFUNCTION(Server, Reliable, WithValidation) void Server_DoorToggle();
	bool Server_DoorToggle_Validate();
	void Server_DoorToggle_Implementation();
	UFUNCTION(NetMulticast, unreliable) void SentOwner(AActor* DoorOwner);
	void SentOwner_Implementation(AActor* DoorOwner);
	UFUNCTION(NetMultiCast, Reliable, WithValidation) void Multi_DoorToggle();
	bool Multi_DoorToggle_Validate();
	void Multi_DoorToggle_Implementation();

public:	
	// Called every frame
	UFUNCTION()void DoorToggle();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)void RealToggleDoor();
	void EventBroadcastFunction(AMultiPlayerCPPCharacter* Character);
	bool Interaction_Implementation(AMultiPlayerCPPCharacter* Character);
	UFUNCTION()void SetAuthority();
};
