// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMPTPSHUD.h"
#include "Blueprint/UserWidget.h"

ATestMPTPSHUD::ATestMPTPSHUD()
{
	static ConstructorHelpers::FClassFinder<UMainUI_Base> MainWidgetUIHelper(TEXT("/Game/UI/WBP_MainUI"));
	if (IsValid(MainWidgetUIHelper.Class))
	{
		MainWidgetUIClass = MainWidgetUIHelper.Class;
	}
}

void ATestMPTPSHUD::BeginPlay()
{
	if (IsValid(MainWidgetUIClass))
	{
		MainWidgetUI = CreateWidget<UMainUI_Base>(GetGameInstance(), MainWidgetUIClass);
		MainWidgetUI->AddToViewport();
	}
}