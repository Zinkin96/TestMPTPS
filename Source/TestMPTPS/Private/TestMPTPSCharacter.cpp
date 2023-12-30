// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestMPTPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Items/Weapons/Weapon_Base_Ranged.h"
#include "Interfaces/Interactable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TestMPTPSPlayerController.h"
#include "TestMPTPSGameState.h"
#include "Kismet/GameplayStatics.h"
#include "TestMPTPSHUD.h"
#include <Net/UnrealNetwork.h>

//////////////////////////////////////////////////////////////////////////
// ATestMPTPSCharacter

ATestMPTPSCharacter::ATestMPTPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel1, ECollisionResponse::ECR_Ignore);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	GetMesh()->SetCollisionProfileName("Ragdoll", true);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

ATestMPTPSCharacter::~ATestMPTPSCharacter()
{
	OnHealthChanged.RemoveAll(this);
	OnWeaponChanged.RemoveAll(this);
}

void ATestMPTPSCharacter::ServerSetAimOffset_Implementation(float Offset_Y, float Offset_Z)
{
	AimOffset_Y = Offset_Y;
	AimOffset_Z = Offset_Z;
}

void ATestMPTPSCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATestMPTPSCharacter, WeaponRef);
	DOREPLIFETIME(ATestMPTPSCharacter, AimOffset_Y);
	DOREPLIFETIME(ATestMPTPSCharacter, AimOffset_Z);
	DOREPLIFETIME(ATestMPTPSCharacter, CharacterStats);
}

void ATestMPTPSCharacter::Tick(float DeltaSeconds)
{
	if (IsLocallyControlled())
	{
		AimOffsetRotator = CalculateLookAtRotation(FRotator(AimOffsetRotator.Pitch, AimOffsetRotator.Yaw, 0.0f), DeltaSeconds);
		ServerSetAimOffset(AimOffsetRotator.Pitch, AimOffsetRotator.Yaw);
	}
}

float ATestMPTPSCharacter::GetAimOffset_Y() const
{
	if (IsLocallyControlled())
	{
		return AimOffsetRotator.Pitch;
	}
	else
	{
		return AimOffset_Y;
	}
}

float ATestMPTPSCharacter::GetAimOffset_Z() const
{
	if (IsLocallyControlled())
	{
		return AimOffsetRotator.Yaw;
	}
	else
	{
		return AimOffset_Z;
	}
}

void ATestMPTPSCharacter::RecalculateHealth()
{
	OnHealthChanged.Broadcast();
	if (GetStats().CurrentHealth <= 0.0f)
	{
		if (IsLocallyControlled())
		{
			if (IsValid(PlayerController))
			{
				DisableInput(PlayerController);
				EndAttack();
				PlayerController->ServerCallRespawnCountdown();
			}
		}
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionProfileName("NoCollision", true);
	}
}

void ATestMPTPSCharacter::WeaponChanged()
{
	OnWeaponChanged.Broadcast();
}

void ATestMPTPSCharacter::ApplyDamage(float Damage, ACharacter* DamageOwner)
{
	if (HasAuthority())
	{
		CharacterStats.CurrentHealth = UKismetMathLibrary::Max(0.0f, CharacterStats.CurrentHealth - Damage);
		if ((CharacterStats.CurrentHealth <= 0.0f) && GameState && bDead == false)
		{
			bDead = true;
			GameState->IncreaseKillCount(DamageOwner->GetPlayerState());
			GameState->IncreaseDeathCount(this->GetPlayerState());
		}
	}
}

void ATestMPTPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	GameState = Cast<ATestMPTPSGameState>(GetWorld()->GetGameState());
	PlayerController = Cast<ATestMPTPSPlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		GameHUD = Cast<ATestMPTPSHUD>(PlayerController->GetHUD());
	}
	//Add Input Mapping Context
	if (IsValid(PlayerController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//Set Default Weapon
	EquipWeapon(StartWeapon);
}

void ATestMPTPSCharacter::EquipWeapon(TSubclassOf<AWeapon_Base_Ranged> WeaponClass, AActor* PickUpActor)
{
	if (IsValid(WeaponClass))
	{
		ServerEquipWeapon(WeaponClass, PickUpActor);
	}
}

void ATestMPTPSCharacter::ServerEquipWeapon_Implementation(TSubclassOf<AWeapon_Base_Ranged> WeaponClass, AActor* PickUpActor)
{
	if (IsValid(PickUpActor))
	{
		PickUpActor->Destroy();
	}
	if (IsValid(WeaponRef))
	{
		EndAttack();
		WeaponRef->Destroy(true);
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetController();
	SpawnParameters.Instigator = this;
	TObjectPtr<AWeapon_Base_Ranged> SpawnedWeapon = GetWorld()->SpawnActor<AWeapon_Base_Ranged>(WeaponClass, FVector().ZeroVector, FRotator().ZeroRotator, SpawnParameters);
	SpawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), "Weapon_R_Socket");
	WeaponRef = SpawnedWeapon;
}

//////////////////////////////////////////////////////////////////////////
// Input
void ATestMPTPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATestMPTPSCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATestMPTPSCharacter::Look);

		//Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ATestMPTPSCharacter::Interact);

		//Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ATestMPTPSCharacter::BeginAttack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ATestMPTPSCharacter::EndAttack);

		//Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ATestMPTPSCharacter::Reload);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ATestMPTPSCharacter::BeginAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATestMPTPSCharacter::EndAim);

		//Scoreboard
		EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Started, this, &ATestMPTPSCharacter::OpenScoreboard);
		EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Completed, this, &ATestMPTPSCharacter::CloseScoreboard);
	}

}

void ATestMPTPSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATestMPTPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATestMPTPSCharacter::Interact(const FInputActionValue& Value)
{
	TObjectPtr<AActor> InteractableActor = IInteractable::Execute_TargetResult_Actor(GetController());
	if (IsValid(InteractableActor) && Cast<IInteractable>(InteractableActor))
	{
		IInteractable::Execute_InteractionEvent(InteractableActor, this);
	}
}

void ATestMPTPSCharacter::BeginAttack()
{
	if (!(GetMesh()->GetAnimInstance()->GetActiveInstanceForMontage(WeaponRef->GetReloadMontage())))
	{
		WeaponRef->BeginAttack();
	}
}

void ATestMPTPSCharacter::EndAttack()
{
	WeaponRef->EndAttack();
}

void ATestMPTPSCharacter::Reload()
{
	if (!IsValid(WeaponRef))
	{
		return;
	}
	if (WeaponRef->GetAmmoStats().CurrentAmmo >= WeaponRef->GetAmmoStats().MaxAmmo)
	{
		return;
	}
	if (GetMesh()->GetAnimInstance()->GetActiveInstanceForMontage(WeaponRef->GetReloadMontage()))
	{
		return;
	}
	if (GetWorldTimerManager().GetTimerRemaining(ReloadTimer) > 0.0f)
	{
		return;
	}
	PlayReloadAnimation();
	EndAttack();
	EndAim();
	GetWorldTimerManager().SetTimer(ReloadTimer, WeaponRef.Get(), &AWeapon_Base_Ranged::RefillMagazine, WeaponRef->GetAmmoStats().ReloadSpeed, false);
	ServerReload();
}

void ATestMPTPSCharacter::BeginAim()
{
	if (!IsValid(WeaponRef))
	{
		return;
	}
	if (GetMesh()->GetAnimInstance()->GetActiveInstanceForMontage(WeaponRef->GetReloadMontage()))
	{
		return;
	}
	if (GetWorldTimerManager().GetTimerRemaining(ReloadTimer) > 0.0f)
	{
		return;
	}
	bAiming = true;
	if (IsValid(PlayerController))
	{
		PlayerController->PlayerCameraManager->SetFOV(75.0f);
	}
	ServerSetAim(true);
}

void ATestMPTPSCharacter::EndAim()
{
	bAiming = false;
	if (IsValid(PlayerController))
	{
		PlayerController->PlayerCameraManager->SetFOV(90.0f);
	}
	ServerSetAim(false);
}

void ATestMPTPSCharacter::OpenScoreboard()
{
	if (IsValid(GameHUD))
	{
		GameHUD->GetMainWidget()->ShowScoreboard(true);
	}
}

void ATestMPTPSCharacter::CloseScoreboard()
{
	if (IsValid(GameHUD))
	{
		GameHUD->GetMainWidget()->ShowScoreboard(false);
	}
}

FRotator ATestMPTPSCharacter::CalculateLookAtRotation(const FRotator AimOffsetCurrent, const float InterpTime, const float InterpSpeed)
{
	if (WeaponRef && GetController())
	{
		FTransform MuzzleSocketTransform = WeaponRef->GetRootComponent()->GetSocketTransform("MuzzleFlash");
		MuzzleSocketTransform = UKismetMathLibrary::MakeRelativeTransform(MuzzleSocketTransform, GetMesh()->GetSocketTransform("Weapon_R_Socket"));
		MuzzleSocketTransform.SetLocation(FVector(0, 0, MuzzleSocketTransform.GetLocation().Z));
		MuzzleSocketTransform = UKismetMathLibrary::ComposeTransforms(MuzzleSocketTransform, GetMesh()->GetSocketTransform("Weapon_R_Socket"));
		FRotator RotateResult = UKismetMathLibrary::FindLookAtRotation(MuzzleSocketTransform.GetLocation(), IInteractable::Execute_TargetResult_Location(GetController()));
		RotateResult = FRotator(RotateResult.Pitch, UKismetMathLibrary::NormalizedDeltaRotator(RotateResult, GetActorRotation()).Yaw, RotateResult.Roll);
		float Delta = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(RotateResult, AimOffsetCurrent).Yaw);
		float InterpSpeedMultiplier = UKismetMathLibrary::MapRangeClamped(Delta, 0, AimOffsetAccelerationAngleThreshold, AimOffsetAccelerationMultiplier.X, AimOffsetAccelerationMultiplier.Y);
		RotateResult = UKismetMathLibrary::RInterpTo(AimOffsetCurrent, RotateResult, InterpTime, InterpSpeed * InterpSpeedMultiplier);
		return RotateResult;
	}
	else
	{
		return FRotator().ZeroRotator;
	}
}

void ATestMPTPSCharacter::CheckHit_Implementation(FVector HitLocation, AActor* HitActor, FVector CollisionSize)
{
	ServerHitCheck(HitLocation, HitActor, CollisionSize);
}

void ATestMPTPSCharacter::ServerHitCheck_Implementation(FVector HitLocation, AActor* HitActor, FVector CollisionSize)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	TArray<AActor*> OverlapedActors;
	UKismetSystemLibrary::BoxOverlapActors(GetWorld(), HitLocation, CollisionSize * 1.2, ObjectTypeQuery, AActor::GetClass(), ActorsToIgnore, OverlapedActors);
	if (!OverlapedActors.Contains(HitActor))
	{
		return;
	}
	if (ATestMPTPSCharacter* HittedCharacter = Cast<ATestMPTPSCharacter>(HitActor))
	{
		HittedCharacter->ApplyDamage(WeaponRef->GetStats().Damage, this);
	}
}

void ATestMPTPSCharacter::ServerReload_Implementation()
{
	MulticastReload();
}

void ATestMPTPSCharacter::MulticastReload_Implementation()
{
	if (!IsLocallyControlled())
	{
		PlayReloadAnimation();
	}
}

void ATestMPTPSCharacter::PlayReloadAnimation()
{
	GetMesh()->GetAnimInstance()->Montage_Play(WeaponRef->GetReloadMontage(), WeaponRef->GetReloadMontage()->GetPlayLength() / WeaponRef->GetAmmoStats().ReloadSpeed);
}

void ATestMPTPSCharacter::ServerSetAim_Implementation(bool Aiming)
{
	MulticastSetAim(Aiming);
}

void ATestMPTPSCharacter::MulticastSetAim_Implementation(bool Aiming)
{
	if (!IsLocallyControlled() && !HasAuthority())
	{
		bAiming = Aiming;
	}
}