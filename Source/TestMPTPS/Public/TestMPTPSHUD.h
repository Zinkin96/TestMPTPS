// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/MainUI_Base.h"
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

	UFUNCTION(BlueprintCallable)
		FORCEINLINE UMainUI_Base* GetMainWidget() { return MainWidgetUI; }

private:

	TObjectPtr<UMainUI_Base> MainWidgetUI;

	TSubclassOf<UUserWidget> MainWidgetUIClass;
};
