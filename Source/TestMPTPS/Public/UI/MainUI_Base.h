// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI_Base.generated.h"

/**
 * 
 */
UCLASS()
class TESTMPTPS_API UMainUI_Base : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowScoreboard(bool bShow);
};
