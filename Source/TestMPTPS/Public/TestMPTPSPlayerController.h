// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/Interactable.h"
#include "TestMPTPSPlayerController.generated.h"

/**
 *
 */
UCLASS()
class TESTMPTPS_API ATestMPTPSPlayerController : public APlayerController, public IInteractable
{
	GENERATED_BODY()

private:

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPossess(APawn* InPawn) override;

	TObjectPtr<class ATestMPTPSCharacter> ControlledCharacterBase;

	FHitResult HitResultTarget;

	FCollisionQueryParams CollisionQueryParamsTarget;

	FVector TargetLocation;

	TObjectPtr<class ATestMPTPSGameMode> GameMode;

	UFUNCTION(BlueprintCallable)
		void CallRespawn();

public:

	ATestMPTPSPlayerController();

	virtual FVector TargetResult_Location_Implementation() override;

	virtual AActor* TargetResult_Actor_Implementation() override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void ServerCallRespawnCountdown();

	virtual void ServerCallRespawnCountdown_Implementation();
};
