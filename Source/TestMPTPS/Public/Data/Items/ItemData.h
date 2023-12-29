// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Items/Item_Base.h"
#include "ItemData.generated.h"

UCLASS()
class TESTMPTPS_API UItemData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Name")
		FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Description")
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Icon")
		UMaterialInstance* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Full stack size")
		int FullStackSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Class")
		TSubclassOf<AItem_Base> Class;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "World mesh")
		UStaticMesh* WorldMesh;

};
