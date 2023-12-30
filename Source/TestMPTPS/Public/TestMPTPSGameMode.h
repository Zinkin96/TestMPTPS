// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TestMPTPSGameMode.generated.h"

UCLASS(minimalapi)
class ATestMPTPSGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATestMPTPSGameMode();

	UFUNCTION(BlueprintCallable)
		FORCEINLINE float GetRespawnTime() { return RespawnTime; }

	UFUNCTION(BlueprintCallable)
		void Respawn(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
		void Despawn(APlayerController* PlayerController);

private:

	TArray<AActor*> PlayerStarts;

	UPROPERTY()
		float RespawnTime = 3.0f;
};



