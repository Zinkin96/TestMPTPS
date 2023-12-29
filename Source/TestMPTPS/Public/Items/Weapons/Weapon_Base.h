// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item_Base.h"
#include "Weapon_Base.generated.h"

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FWeaponStats
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Damage", DisplayName = "Damage")
		float Damage = 1.0f;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Damage", DisplayName = "Projectile count")
		int32 ProjectileCount = 1;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Attack speed", DisplayName = "Attack rate")
		float AttackRate = 1.0f;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Attack speed", DisplayName = "Attack delay")
		float AttackDelay = 0.0f;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Utility", DisplayName = "Attack spread")
		float AttackSpread = 0.0f;
};

UCLASS(Abstract)
class TESTMPTPS_API AWeapon_Base : public AItem_Base
{
	GENERATED_BODY()

public:

	AWeapon_Base();

	UFUNCTION(BlueprintCallable)
		virtual void BeginAttack();

	UFUNCTION(BlueprintCallable)
		virtual void EndAttack();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE FWeaponStats GetStats() { return WeaponStats; }

protected:

	UPROPERTY()
		FTimerHandle AttackTimer;

	UPROPERTY()
		FTimerHandle AttackChargeTimer;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (AllowPrivateAccess = "true"))
		void AttackHandle();

	UFUNCTION()
		virtual bool IsAttackPossible();

	UPROPERTY()
		bool bWantsToAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Weapon parameters", DisplayName = "Weapon stats")
		FWeaponStats WeaponStats;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"), DisplayName = "Weapon mesh")
		USkeletalMeshComponent* WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RootMesh"));
};
