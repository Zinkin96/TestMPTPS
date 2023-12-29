// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item_Base.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	MainHand	UMETA(DisplayName = "Main hand"),
	OffHand		UMETA(DisplayName = "Offhand"),
	Head		UMETA(DisplayName = "Head"),
	Chest		UMETA(DisplayName = "Chest"),
	Hands		UMETA(DisplayName = "Hands"),
	Feet		UMETA(DisplayName = "Feet"),
	Belt		UMETA(DisplayName = "Belt"),
	Back		UMETA(DisplayName = "Back"),
};

UCLASS(Abstract)
class TESTMPTPS_API AItem_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
