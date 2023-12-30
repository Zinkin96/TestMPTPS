// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestMPTPSGameMode.h"
#include "TestMPTPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "TestMPTPSPlayerController.h"
#include "TestMPTPSHUD.h"
#include "TestMPTPSGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/NavigationObjectBase.h"
#include "Math/UnrealMathUtility.h"

ATestMPTPSGameMode::ATestMPTPSGameMode()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavigationObjectBase::StaticClass(), PlayerStarts);

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = ATestMPTPSPlayerController::StaticClass();
	HUDClass = ATestMPTPSHUD::StaticClass();
	GameStateClass = ATestMPTPSGameState::StaticClass();

}

void ATestMPTPSGameMode::Respawn(APlayerController* PlayerController)
{
	TObjectPtr<AActor> RandomPlayerStart = PlayerStarts[FMath::RandRange(0, PlayerStarts.Num()-1)];
	if (HasAuthority())
	{
		Despawn(PlayerController);

		FActorSpawnParameters SpawnParameters;
		TObjectPtr<ATestMPTPSCharacter> SpawnedCharacter = GetWorld()->SpawnActor<ATestMPTPSCharacter>(DefaultPawnClass, RandomPlayerStart->GetActorLocation(), RandomPlayerStart->GetActorRotation(), SpawnParameters);
		PlayerController->Possess(SpawnedCharacter);
	}
}

void ATestMPTPSGameMode::Despawn(APlayerController* PlayerController)
{
	TObjectPtr<APawn> ControlledPawn = PlayerController->GetPawn();
	PlayerController->UnPossess();
	TArray<AActor*> ControlledPawnChildActors;
	ControlledPawn->GetAttachedActors(ControlledPawnChildActors, true);
	for (AActor* Child : ControlledPawnChildActors)
	{
		Child->Destroy(true);
	}
	ControlledPawn->Destroy();
}
