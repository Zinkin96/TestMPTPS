// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMPTPSPlayerController.h"
#include "TestMPTPSCharacter.h"
#include "TestMPTPSGameMode.h"
#include "TestMPTPSHUD.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ATestMPTPSPlayerController::ATestMPTPSPlayerController()
{
	GameMode = Cast<ATestMPTPSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ATestMPTPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ATestMPTPSPlayerController::Tick(float DeltaSeconds)
{
	FVector StartPoint = PlayerCameraManager->GetCameraLocation();
	FVector EndPoint = PlayerCameraManager->GetCameraLocation() + (PlayerCameraManager->GetActorForwardVector() * 10000.0f);
	GetWorld()->LineTraceSingleByChannel(HitResultTarget, StartPoint, EndPoint, ECollisionChannel::ECC_Visibility, CollisionQueryParamsTarget, FCollisionResponseParams::DefaultResponseParam);
	/*Debug raycast */
	//if (HitResultTarget.bBlockingHit)
	//{
	//	DrawDebugLine(GetWorld(), StartPoint, HitResultTarget.Location, FColor::Red, false, -1.0f);
	//	DrawDebugLine(GetWorld(), HitResultTarget.Location, EndPoint, FColor::Emerald, false, -1.0f);
	//	DrawDebugPoint(GetWorld(), HitResultTarget.Location, 10, FColor::Red, false, -1.0f);
	//}
	//else
	//{
	//	DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Emerald, false, -1.0f);
	//}
	/**/
	if (HitResultTarget.bBlockingHit)
	{
		TargetLocation = HitResultTarget.Location;
	}
	else
	{
		TargetLocation = HitResultTarget.TraceEnd;
	}
}

void ATestMPTPSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CollisionQueryParamsTarget.ClearIgnoredActors();
	CollisionQueryParamsTarget.AddIgnoredActor(GetPawn());
	ControlledCharacterBase = Cast<ATestMPTPSCharacter>(GetPawn());
}

FVector ATestMPTPSPlayerController::TargetResult_Location_Implementation()
{
	return TargetLocation;
}

AActor* ATestMPTPSPlayerController::TargetResult_Actor_Implementation()
{
	return HitResultTarget.GetActor();
}

void ATestMPTPSPlayerController::ServerCallRespawnCountdown_Implementation()
{
	if (!IsValid(GameMode))
	{
		return;
	}
	FTimerHandle RespawnTimer;
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ATestMPTPSPlayerController::CallRespawn, GameMode->GetRespawnTime(), false);
}

void ATestMPTPSPlayerController::ClientRoundEnd_Implementation()
{
	if (GetPawn())
	{
		GetPawn()->GetMovementComponent()->StopMovementImmediately();
	}
	UnPossess();
	if (TObjectPtr<ATestMPTPSHUD> GameHUD = Cast<ATestMPTPSHUD>(GetHUD()))
	{
		GameHUD->GetMainWidget()->ShowScoreboard(true);
	}
}

void ATestMPTPSPlayerController::CallRespawn()
{
	if (!IsValid(GameMode))
	{
		return;
	}
	GameMode->Respawn(this);
}