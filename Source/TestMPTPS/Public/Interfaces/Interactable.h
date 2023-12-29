// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class TESTMPTPS_API IInteractable
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Trigger Reaction")
		void InteractionEvent(AActor* InteractionCaller);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Trigger Reaction")
		FVector TargetResult_Location();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Trigger Reaction")
		AActor* TargetResult_Actor();
};
