// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickUp.h"
#include "TestMPTPSCharacter.h"
#include "Items/Weapons/Weapon_Base_Ranged.h"

// Sets default values
APickUp::APickUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// Set Replication
	SetReplicates(true);

	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = ItemMesh;
	ItemMesh->SetStaticMesh(NULL);
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetUseCCD(true);
	ItemMesh->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->BodyInstance.SetResponseToAllChannels(ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();
	ConstructPickupMesh();
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUp::ConstructPickupMesh()
{
	if (!IsValid(Item))
	{
		return;
	}
	if (!IsValid(Item->WorldMesh))
	{
		return;
	}
	ItemMesh->SetStaticMesh(Item->WorldMesh);
	return;
}

void APickUp::InteractionEvent_Implementation(AActor* InteractionCaller)
{
	TSubclassOf<AWeapon_Base_Ranged> WeaponToEquip { Item->Class };
	if (ATestMPTPSCharacter* CharacterReference = Cast<ATestMPTPSCharacter>(InteractionCaller))
	{
		CharacterReference->EquipWeapon(WeaponToEquip);
	}
	Destroy();
	return;
}

#if WITH_EDITOR
void APickUp::OnConstruction(const FTransform& FTransform)
{
	ConstructPickupMesh();
}
#endif