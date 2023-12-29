// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TestMPTPSHUD.generated.h"

/**
 * 
 */
UCLASS()
class TESTMPTPS_API ATestMPTPSHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATestMPTPSHUD();

	virtual void BeginPlay() override;

private:

	TObjectPtr<UUserWidget> MainWidgetUI;

	TSubclassOf<UUserWidget> MainWidgetUIClass;
};
