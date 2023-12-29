// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon_Base.h"

AWeapon_Base::AWeapon_Base()
{
	RootComponent = WeaponMesh;
}

void AWeapon_Base::BeginAttack()
{

}

void AWeapon_Base::EndAttack()
{

}

bool AWeapon_Base::IsAttackPossible()
{
	if (GetWorldTimerManager().GetTimerRemaining(AttackTimer) > 0.0f)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void AWeapon_Base::AttackHandle_Implementation()
{

}