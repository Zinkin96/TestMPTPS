// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Items/ItemData.h"
#include "Interfaces/Interactable.h"
#include "PickUp.generated.h"

UCLASS()
class TESTMPTPS_API APickUp : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUp();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), DisplayName = "Item")
		TObjectPtr<UItemData> Item;

	virtual void InteractionEvent_Implementation(AActor* InteractionCaller) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UStaticMeshComponent* ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));

	void ConstructPickupMesh();


	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

#if WITH_EDITOR
	void OnConstruction(const FTransform& Transform) override;
#endif
};
