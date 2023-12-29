// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Base.h"
#include "Interfaces/Damageable.h"

// Sets default values
AProjectile_Base::AProjectile_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(ProjectileCollision);
	ParticleSystem->SetupAttachment(RootComponent);
	InitialLifeSpan = 3.0f;
	ProjectileCollision->SetBoxExtent(CollisionSize,false);
	ProjectileCollision->OnComponentHit.AddDynamic(this, &AProjectile_Base::ProjectileHit);
	ProjectileCollision->SetCollisionObjectType(ECollisionChannel::ECC_EngineTraceChannel1);
	ProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	ProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	ProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	ProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	ProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	ProjectileCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
	ProjectileCollision->SetUseCCD(true);
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

// Called when the game starts or when spawned
void AProjectile_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile_Base::ProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetInstigator()->IsLocallyControlled() && IsValid(Hit.GetActor()))
	{
		IDamageable::Execute_CheckHit(GetInstigator(), Hit.Location, Hit.GetActor(), CollisionSize);
	}
	Destroy();
}