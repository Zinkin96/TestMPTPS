// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Delegates/Delegate.h"
#include "TestMPTPSGameState.generated.h"

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FPlayerStat
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Stat", DisplayName = "Player")
		TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Stat", DisplayName = "Kills")
		int32 Kills = 0;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Stat", DisplayName = "Deaths")
		int32 Deaths = 0;

public:
	FPlayerStat()
	{
		PlayerState = nullptr;
		Kills = 0;
		Deaths = 0;
	}

	FPlayerStat(TObjectPtr<APlayerState> NewPlayerState)
	{
		PlayerState = NewPlayerState;
		Kills = 0;
		Deaths = 0;
	}
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChangedDelegate, float, Time);
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayersStatChangedDelegate, const TArray<FPlayerStat>&, PlayersStats);

UCLASS()
class TESTMPTPS_API ATestMPTPSGameState : public AGameState
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, ReplicatedUsing = TimeChanged, meta = (AllowPrivateAccess = "true"), DisplayName = "Round length")
		float RoundLength = 300.0f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, ReplicatedUsing = StatChanged, meta = (AllowPrivateAccess = "true"), DisplayName = "Players stat")
		TArray<FPlayerStat> PlayersStat;

private:

	FTimerHandle CountdownTimer;

	void DecreaseTime();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	UPROPERTY(BlueprintAssignable)
		FOnTimeChangedDelegate OnTimeChanged;

	UPROPERTY(BlueprintAssignable)
		FOnPlayersStatChangedDelegate OnPlayersStatChanged;

	UFUNCTION(BlueprintCallable)
		void IncreaseKillCount(APlayerState* PlayerState);

	UFUNCTION(BlueprintCallable)
		void IncreaseDeathCount(APlayerState* PlayerState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetTime() { return RoundLength; }

	UFUNCTION()
		void TimeChanged();

	UFUNCTION()
		void StatChanged();
};
