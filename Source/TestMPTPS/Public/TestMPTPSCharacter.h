// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interfaces/Damageable.h"
#include "Delegates/Delegate.h"
#include "TestMPTPSCharacter.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChangedDelegate);
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponChangedDelegate);

USTRUCT(BlueprintType, Blueprintable)
struct FCharacterStats
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Health", DisplayName = "Base health")
		float BaseHealth = 100.0f;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Health", DisplayName = "Current health")
		float CurrentHealth = BaseHealth;
};

UCLASS(config = Game)
class TESTMPTPS_API ATestMPTPSCharacter : public ACharacter, public IDamageable
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* InteractAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AttackAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ReloadAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AimAction;

	/** Scoreboard Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ScoreboardAction;

public:
	ATestMPTPSCharacter();

	~ATestMPTPSCharacter();

	UFUNCTION(BlueprintCallable)
		virtual FRotator CalculateLookAtRotation(const FRotator AimOffsetCurrent, const float InterpTime = 0.0f, const float InterpSpeed = 15.0f);

	UFUNCTION(BlueprintCallable)
		virtual void EquipWeapon(TSubclassOf<AWeapon_Base_Ranged> WeaponClass, AActor* PickUpActor = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FCharacterStats GetStats() { return CharacterStats; }

	UPROPERTY(BlueprintAssignable)
		FOnHealthChangedDelegate OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
		FOnWeaponChangedDelegate OnWeaponChanged;

	virtual void CheckHit_Implementation(FVector HitLocation, AActor* HitActor, FVector CollisionSize) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Interact(const FInputActionValue& Value);

	void BeginAttack();

	void EndAttack();

	void Reload();

	void BeginAim();

	void EndAim();

	void OpenScoreboard();

	void CloseScoreboard();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"), DisplayName = "Aim offset acceleration angle threshold")
		float AimOffsetAccelerationAngleThreshold = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"), DisplayName = "Aim offset acceleration multiplier")
		FVector2D AimOffsetAccelerationMultiplier = FVector2D(1.0f, 2.0f);

	UPROPERTY(Replicated, BlueprintReadWrite, ReplicatedUsing = WeaponChanged, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class AWeapon_Base_Ranged> WeaponRef;

	UPROPERTY(Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Aim Offset", DisplayName = "Aim Offset Pitch")
		float AimOffset_Y = 0.0f;

	UPROPERTY(Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Aim Offset", DisplayName = "Aim Offset Yaw")
		float AimOffset_Z = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), DisplayName = "Start weapon")
		TSubclassOf<AWeapon_Base_Ranged> StartWeapon;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, ReplicatedUsing = RecalculateHealth, meta = (AllowPrivateAccess = "true"), DisplayName = "Character stats")
		FCharacterStats CharacterStats;

	//Net functions
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void ServerEquipWeapon(TSubclassOf<AWeapon_Base_Ranged> WeaponClass, AActor* PickUpActor = nullptr);

	virtual void ServerEquipWeapon_Implementation(TSubclassOf<AWeapon_Base_Ranged> WeaponClass, AActor* PickUpActor = nullptr);

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void ServerSetAimOffset(float Offset_Y, float Offset_Z);

	virtual void ServerSetAimOffset_Implementation(float Offset_Y, float Offset_Z);

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void ServerHitCheck(FVector HitLocation, AActor* HitActor, FVector CollisionSize);

	virtual void ServerHitCheck_Implementation(FVector HitLocation, AActor* HitActor, FVector CollisionSize);

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void ServerReload();

	virtual void ServerReload_Implementation();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
		void MulticastReload();

	virtual void MulticastReload_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void ServerSetAim(bool Aiming);

	virtual void ServerSetAim_Implementation(bool Aiming);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
		void MulticastSetAim(bool Aiming);

	virtual void MulticastSetAim_Implementation(bool Aiming);

	UFUNCTION(BlueprintCallable)
		void ApplyDamage(float Damage, ACharacter* DamageOwner);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
		float GetAimOffset_Y() const;

	UFUNCTION(BlueprintCallable)
		float GetAimOffset_Z() const;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, DisplayName = "Aiming")
		bool bAiming;

private:

	FRotator AimOffsetRotator;

	FTimerHandle ReloadTimer;

	TObjectPtr<class ATestMPTPSGameState> GameState;

	TObjectPtr<class ATestMPTPSPlayerController> PlayerController;

	TObjectPtr<class ATestMPTPSHUD> GameHUD;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION()
		void RecalculateHealth();

	UFUNCTION()
		void WeaponChanged();

	UFUNCTION()
		void PlayReloadAnimation();

	bool bDead = false;

};

