// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon_Base_Ranged.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile_Base.h"
#include "Animation/AnimMontage.h"
#include "Interfaces/Interactable.h"

void AWeapon_Base_Ranged::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon_Base_Ranged::BeginAttack()
{
	bWantsToAttack = true;
	if (WeaponStats.AttackDelay <= 0)
	{
		AttackHandle();
	}
	else
	{
		GetWorldTimerManager().SetTimer(AttackChargeTimer, this, &AWeapon_Base_Ranged::AttackHandle, WeaponStats.AttackDelay, false);
	}
}

void AWeapon_Base_Ranged::EndAttack()
{
	bWantsToAttack = false;
}

void AWeapon_Base_Ranged::AttackHandle_Implementation()
{
	if (bWantsToAttack && (GetWorldTimerManager().GetTimerRemaining(AttackTimer) <= 0.0f))
	{
		if (IsAttackPossible())
		{
			FVector StartLocation = WeaponMesh->GetSocketLocation("MuzzleFlash");

			FRotator ResultRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, IInteractable::Execute_TargetResult_Location(GetOwner())) + FRotator(UKismetMathLibrary::RandomFloatInRange(WeaponStats.AttackSpread / 2 * -1, WeaponStats.AttackSpread / 2), UKismetMathLibrary::RandomFloatInRange(WeaponStats.AttackSpread / 2 * -1, WeaponStats.AttackSpread / 2), 0);
			UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, RootComponent, "MuzzleFlash");

			WeaponAmmoStats.CurrentAmmo = FMath::Max(0, WeaponAmmoStats.CurrentAmmo-1);
			SpawnProjectile(StartLocation, ResultRotation);
			ServerWeaponAttack(StartLocation, ResultRotation);
		}

		GetWorldTimerManager().SetTimer(AttackTimer, this, &AWeapon_Base_Ranged::AttackHandle, WeaponStats.AttackRate, false);
	}
}

bool AWeapon_Base_Ranged::IsAttackPossible()
{
	if (WeaponAmmoStats.CurrentAmmo <= 0)
	{
		return false;
	}
	Super::IsAttackPossible();
	return true;
}

void AWeapon_Base_Ranged::ServerWeaponAttack_Implementation(FVector Location, FRotator Rotation)
{
	MulticastWeaponAttack(Location, Rotation);
}

void AWeapon_Base_Ranged::MulticastWeaponAttack_Implementation(FVector Location, FRotator Rotation)
{
	if (!GetInstigator()->IsLocallyControlled() && !(GetInstigator()->HasAuthority()))
	{
		SpawnProjectile(Location, Rotation);
	}
}

void AWeapon_Base_Ranged::SpawnProjectile(FVector Location, FRotator Rotation)
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, RootComponent, "MuzzleFlash");
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = GetInstigator();
	GetWorld()->SpawnActor<AProjectile_Base>(ProjectileClass, Location, Rotation, SpawnParameters);
}

void AWeapon_Base_Ranged::RefillMagazine()
{
	WeaponAmmoStats.CurrentAmmo = WeaponAmmoStats.MaxAmmo;
}
