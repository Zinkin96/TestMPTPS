// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMPTPSGameState.h"
#include "GameFramework/PlayerState.h"
#include "TestMPTPSGameMode.h"
#include "TestMPTPSPlayerController.h"
#include "GameFramework/PawnMovementComponent.h"
#include <Net/UnrealNetwork.h>

void ATestMPTPSGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATestMPTPSGameState, RoundLength);
	DOREPLIFETIME(ATestMPTPSGameState, PlayersStat);
}

void ATestMPTPSGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	bool Contain = PlayersStat.ContainsByPredicate([&](const FPlayerStat& Stat) {
		return Stat.PlayerState.Get() == PlayerState; });
	if (!Contain)
	{
		PlayersStat.Add(FPlayerStat(PlayerState));
	}
}

void ATestMPTPSGameState::IncreaseKillCount(APlayerState* PlayerState)
{
	FPlayerStat* PlayerStat = PlayersStat.FindByPredicate([&](const FPlayerStat& Stat) {
		return Stat.PlayerState.Get() == PlayerState;});
	PlayerStat->Kills = ++PlayerStat->Kills;
}

void ATestMPTPSGameState::IncreaseDeathCount(APlayerState* PlayerState)
{
	FPlayerStat* PlayerStat = PlayersStat.FindByPredicate([&](const FPlayerStat& Stat) {
		return Stat.PlayerState.Get() == PlayerState; });
	PlayerStat->Deaths = ++PlayerStat->Deaths;
}

void ATestMPTPSGameState::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(CountdownTimer, this, &ATestMPTPSGameState::DecreaseTime, 1.0f, true);
	}
}

void ATestMPTPSGameState::DecreaseTime()
{
	RoundLength = FMath::Max(0, --RoundLength);
	if (RoundLength > 0)
	{
		return;
	}
	ATestMPTPSGameMode* GameMode = Cast<ATestMPTPSGameMode>(GetWorld()->GetAuthGameMode());
	for (TObjectPtr<APlayerState> Player : PlayerArray)
	{
		if (TObjectPtr<ATestMPTPSPlayerController> PlayerController = Cast<ATestMPTPSPlayerController>(Player->GetPlayerController()))
		{
			if (PlayerController->GetPawn())
			{
				PlayerController->GetPawn()->GetMovementComponent()->StopMovementImmediately();
			}
			PlayerController->UnPossess();
			PlayerController->ClientRoundEnd();
		}
	}
}

void ATestMPTPSGameState::TimeChanged()
{
	OnTimeChanged.Broadcast(RoundLength);
}

void ATestMPTPSGameState::StatChanged()
{
	OnPlayersStatChanged.Broadcast(PlayersStat);
}
