// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Projectile_Base.generated.h"

UCLASS(Abstract)
class TESTMPTPS_API AProjectile_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	FVector CollisionSize = FVector(3.0f, 3.0f, 3.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), DisplayName = "Projectile movement")
		UProjectileMovementComponent* ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile movement"));

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), DisplayName = "Projectile collision")
		UBoxComponent* ProjectileCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Projectile collision"));

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), DisplayName = "Particle system")
		UParticleSystemComponent* ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle system"));

	UFUNCTION()
		void ProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
