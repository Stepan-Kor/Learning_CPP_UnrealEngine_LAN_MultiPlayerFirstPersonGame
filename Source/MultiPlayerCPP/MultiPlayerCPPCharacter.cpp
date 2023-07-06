// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiPlayerCPPCharacter.h"
#include "MultiPlayerCPPProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Net/UnrealNetwork.h"
#include "Components/TextRenderComponent.h"
#include "InteractionInterface.h"
#include "Door.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMultiPlayerCPPCharacter

AMultiPlayerCPPCharacter::AMultiPlayerCPPCharacter()
{

	bReplicates = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetIsReplicated(true);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	TextComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_Render_Component"));
	TextComp->SetupAttachment(RootComponent);
	TextComp->SetRelativeLocation(FVector(70, -40, 0));
	TextComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	TextComp->SetText(FText::FromString ("0"));

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AMultiPlayerCPPCharacter::OnRep_OnFire()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Shots changed function ");
	if (IsValid(TextComp)) 	TextComp->SetText(FText::FromString(FString::FromInt(ShotsFired)));
	
}

void AMultiPlayerCPPCharacter::ExecuteShot(FRotator Rotation, APlayerController* PlayerController)
{	

	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Emerald, FString::FromInt(ShotsFired));
	//if (IsValid(TextComp)) TextComp->SetText(FString::FromInt(ShotsFired));
	if (IsValid(TextComp)) { 
		//TextComp->SetText(FString::FromInt(ShotsFired));
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Emerald, "Text comp valid");
	}
	else { 
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Purple, "No text component in variable"); 

		//TextComp=Cast<UTextRenderComponent>( AMultiPlayerCPPCharacter::GetComponentByClass(TSubclassOf<UActorComponent> UTextRenderComponent::StaticClass));
		TextComp=Cast<UTextRenderComponent>(AMultiPlayerCPPCharacter::GetComponentByClass(UTextRenderComponent::StaticClass()));
		if (IsValid(TextComp)) {
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Emerald, "Now Text comp valid");
			TextComp->SetText(FText::FromString(FString::FromInt(ShotsFired)));
		}
		else GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Emerald, "STill Text comp is not valid");


		//TArray<UTextRenderComponent> TextComps;
		//AMultiPlayerCPPCharacter->GetComponents** <UStaticMeshComponent>** (TextComps);



	}


	//GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Orange, FString::FromInt(ShotsFired));
	// try and fire a projectile



	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				//const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				//const FRotator SpawnRotation = Rotation;
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AMultiPlayerCPPProjectile>(ProjectileClass, SpawnLocation, Rotation);
			}
			else
			{
				//const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + Rotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				ActorSpawnParams.Owner = PlayerController;

				// spawn the projectile at the muzzle
				World->SpawnActor<AMultiPlayerCPPProjectile>(ProjectileClass, SpawnLocation, Rotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AMultiPlayerCPPCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}


}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiPlayerCPPCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMultiPlayerCPPCharacter::OnFire);
	PlayerInputComponent->BindAction("Interact",IE_Pressed,this,&AMultiPlayerCPPCharacter::InteractTrace);
	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMultiPlayerCPPCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiPlayerCPPCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiPlayerCPPCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMultiPlayerCPPCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMultiPlayerCPPCharacter::LookUpAtRate);
}

void AMultiPlayerCPPCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps)const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME_CONDITION(AMultiPlayerCPPCharacter, ShotsFired, COND_OwnerOnly);
	DOREPLIFETIME(AMultiPlayerCPPCharacter, ShotsFired);
	//DOREPLIFETIME_CONDITION(AMultiPlayerCPPCharacter, ShotsFired, COND_OwnerOnly);
	
}
//void AMultiPlayerCPPCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) 
//{
//	DOREPLIFETIME(AMultiPlayerCPPCharacter, ShotsFired);
//}

void AMultiPlayerCPPCharacter::OnFire()
{
	//this->GetName();
	FRotator Rotation;
	if (bUsingMotionControllers)		Rotation = VR_MuzzleLocation->GetComponentRotation();
	else	Rotation = GetControlRotation();

	Server_OnFire(Rotation,Cast<APlayerController>(GetController()));
	/*if (!HasAuthority())Server_OnFire(Rotation);
	else Multi_OnFire(Rotation);*/
}

void AMultiPlayerCPPCharacter::Server_OnFire_Implementation(FRotator Rotation, APlayerController* PlayerController)
{
	ShotsFired++;
	OnRep_OnFire();
	//Server_NewFunc();
	Multi_OnFire(Rotation,PlayerController);
}

void AMultiPlayerCPPCharacter::Multi_OnFire_Implementation(FRotator Rotation, APlayerController* PlayerController)
{
	ExecuteShot(Rotation,PlayerController);
}

bool AMultiPlayerCPPCharacter::Server_NewFunc_Validate()
{
	return true;
}

void AMultiPlayerCPPCharacter::Server_NewFunc_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "server NewFunc Called ");
}


void AMultiPlayerCPPCharacter::InteractTrace()
{	
	FHitResult HitResultat;
	FVector Start = FP_MuzzleLocation->K2_GetComponentLocation();
	FVector End = Start + FP_MuzzleLocation->GetRightVector() * 800.f;
	//FCollisionObjectQueryParams CollisionObjectParams;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	const FName TraceTag("MyTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;
	CollisionParams.TraceTag = TraceTag;



	GetWorld()->LineTraceSingleByObjectType(HitResultat, Start, End, FCollisionObjectQueryParams(), CollisionParams);
	
	if (HitResultat.bBlockingHit && IsValid(HitResultat.GetActor())) {

		//ADoor* TempDoor =Cast <ADoor>(HitResultat.GetActor());
		//if (IsValid(TempDoor))TempDoor->DoorToggle();
		//if(HasAuthority())HitResultat.GetActor()->SetOwner(GetOwner());
		IInteractionInterface* InteractInterface = Cast<IInteractionInterface>(HitResultat.GetActor());
		if (InteractInterface != nullptr) {
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "local interaction target ok");
			if (!HasAuthority()) Server_Interaction(HitResultat.GetActor());
			else {
				InteractInterface->Execute_Interaction(HitResultat.GetActor(), this);
				//FString TempString = HitResultat.GetActor()->GetName();
				//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green,TempString);
			}
			
		}
	}
}
void AMultiPlayerCPPCharacter::Server_Interaction_Implementation(AActor* target)
{
	if (IsValid(target)) {
		IInteractionInterface* InteractInterface = Cast<IInteractionInterface>(target);
		if (InteractInterface!=nullptr) {
		InteractInterface->Execute_Interaction(target, this);
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("SERVER%i character %s interacted with %s"), this->GetController()->GetLinkerIndex(), *this->GetName(), *target->GetName()));
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("SERVER character %s interacted with %s"), *this->GetName(), *target->GetName()));
		}
	}
	else GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "SERVER interface variable empty");
}
bool AMultiPlayerCPPCharacter::Server_Interaction_Validate(AActor* target){	return true;}


bool AMultiPlayerCPPCharacter::Multi_OnFire_Validate(FRotator Rotation, APlayerController* PlayerController){	return true;}

bool AMultiPlayerCPPCharacter::Server_OnFire_Validate(FRotator Rotation, APlayerController* PlayerController){	return true;}

void AMultiPlayerCPPCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMultiPlayerCPPCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AMultiPlayerCPPCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AMultiPlayerCPPCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AMultiPlayerCPPCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, "movecomand");
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMultiPlayerCPPCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMultiPlayerCPPCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMultiPlayerCPPCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AMultiPlayerCPPCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMultiPlayerCPPCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AMultiPlayerCPPCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMultiPlayerCPPCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}
