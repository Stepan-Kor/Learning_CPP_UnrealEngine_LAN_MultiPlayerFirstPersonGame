// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiPlayerCPPProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "MultiPlayerCPPCharacter.h"
#include "MultiPlayerCPPGameMode.h"
#include "MultiPlayerController.h"
#include "MyPlayerState.h"

AMultiPlayerCPPProjectile::AMultiPlayerCPPProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMultiPlayerCPPProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AMultiPlayerCPPProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()) {
		if(auto MPC = GetOwner<AMultiPlayerController>())PlayerState = MPC->GetPlayerState<AMyPlayerState>() ;
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, FString::Printf(TEXT("ball owner: %s"), *GetOwner()->GetName()));
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, FString::Printf(TEXT("ball owner: %s"), *PlayerState->GetName()));
	}
}

void AMultiPlayerCPPProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
	else if (HasAuthority())	{
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Purple, "server ball hit");
		if(AMultiPlayerCPPCharacter* OtherPlayer=Cast<AMultiPlayerCPPCharacter>(OtherActor)){
			if (AMultiPlayerCPPGameMode* GM = GetWorld()->GetAuthGameMode<AMultiPlayerCPPGameMode>()) {
				GM->PlayerHited();
				if (PlayerState && HasAuthority())PlayerState->PlayerHited(OtherPlayer);

			}
		}
	}
}