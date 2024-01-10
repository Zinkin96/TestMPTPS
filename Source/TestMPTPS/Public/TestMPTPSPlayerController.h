// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/Interactable.h"
#include "Delegates/Delegate.h"
#include "TestMPTPSPlayerController.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetedActorChangedDelegate, AActor*, TargetActor);

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

	TObjectPtr<AActor> TargetActor;

	TObjectPtr<class ATestMPTPSGameMode> GameMode;

	UFUNCTION(BlueprintCallable)
		void CallRespawn();

public:

	virtual void BeginPlay() override;

	ATestMPTPSPlayerController();

	UPROPERTY(BlueprintAssignable)
		FOnTargetedActorChangedDelegate OnTargetedActorChanged;

	virtual FVector TargetResult_Location_Implementation() override { return TargetLocation; }

	virtual AActor* TargetResult_Actor_Implementation() override { return TargetActor; }

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void ServerCallRespawnCountdown();

	virtual void ServerCallRespawnCountdown_Implementation();

	UFUNCTION(Client, Reliable, BlueprintCallable)
		void ClientRoundEnd();

	virtual void ClientRoundEnd_Implementation();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float InteractionRange = 250.0f;
};
