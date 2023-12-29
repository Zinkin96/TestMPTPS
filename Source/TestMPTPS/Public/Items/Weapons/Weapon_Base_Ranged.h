// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/Weapon_Base.h"
#include "Particles/ParticleSystem.h"
#include "Weapon_Base_Ranged.generated.h"

/**
 *
 */
class UAnimMontage;

USTRUCT(BlueprintType, Blueprintable)
struct FAmmoStats
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Ammo", DisplayName = "Max ammo")
		int32 MaxAmmo = 10.0f;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Ammo", DisplayName = "Current ammo")
		int32 CurrentAmmo = MaxAmmo;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Ammo", DisplayName = "Reload speed")
		float ReloadSpeed = 2.0f;
};

UCLASS()
class TESTMPTPS_API AWeapon_Base_Ranged : public AWeapon_Base
{
	GENERATED_BODY()

public:

	void BeginAttack() override;

	void EndAttack() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FAmmoStats GetAmmoStats() { return WeaponAmmoStats; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE UAnimMontage* GetReloadMontage() { return ReloadMontageAsset; }

	UFUNCTION()
		void RefillMagazine();

protected:

	void AttackHandle_Implementation() override;

	virtual bool IsAttackPossible() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Weapon parameters", DisplayName = "Weapon ammo stats")
		FAmmoStats WeaponAmmoStats;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UParticleSystem> MuzzleParticle;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AProjectile_Base> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UAnimMontage> ReloadMontageAsset;

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void ServerWeaponAttack(FVector Location, FRotator Rotation);

	virtual void ServerWeaponAttack_Implementation(FVector Location, FRotator Rotation);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
		void MulticastWeaponAttack(FVector Location, FRotator Rotation);

	virtual void MulticastWeaponAttack_Implementation(FVector Location, FRotator Rotation);

private:

	virtual void BeginPlay() override;

	UFUNCTION()
		void SpawnProjectile(FVector Location, FRotator Rotation);
};
