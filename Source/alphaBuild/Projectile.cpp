// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Main.h"
#include "Enemy.h"
#include "Shield.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	SetRootComponent(HitBox);

	Mesh = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	IdleParticlesComponent = CreateAbstractDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticlesComponent"));
	IdleParticlesComponent->SetupAttachment(GetRootComponent());

	Damage = 10.f;
	KnockBack = 500.f;
	MovementSpeed = 200.f;
	MaxLifeSpan = 5.f;
	CurrentLifeSpan = 0.f;
	bDestroyOnHit = false;
	bHarmsMain = false;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	ForwardVector = GetActorForwardVector();
	
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);

}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentLifeSpan += DeltaTime;
	if (CurrentLifeSpan >= MaxLifeSpan)
	{
		Destroy();
	}
	SetActorLocation(GetActorLocation() + (ForwardVector * DeltaTime * MovementSpeed));
}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (bHarmsMain)
		{
			AMain* Main = Cast<AMain>(OtherActor);
		    if (Main)
			{
				if (!Main->bBlocking)
				{
					FRotator ToTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Main->GetActorLocation());
					FRotator YawToTargetRotation = FRotator(0.f, ToTargetRotation.Yaw, 0.f);
					// get forward vector
					FVector Direction = FRotationMatrix(YawToTargetRotation).GetUnitAxis(EAxis::X);
					Main->TakeDMG(Damage, KnockBack, ForwardVector + FVector(0.f, 0.f, 0.1f));
				}
				OverlapUtility();
			}
		}
		else
		{
			AEnemy* Enemy = Cast<AEnemy>(OtherActor);
			if (Enemy)
			{
				bool Ignore = OtherComp->ComponentHasTag("Ignore");
				if (!Ignore)
				{
					FRotator ToTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Enemy->GetActorLocation());
					FRotator YawToTargetRotation = FRotator(0.f, ToTargetRotation.Yaw, 0.f);
					// get forward vector
					FVector Direction = FRotationMatrix(YawToTargetRotation).GetUnitAxis(EAxis::X);
					Enemy->TakeDMG(Damage, KnockBack, ForwardVector + FVector(0.f, 0.f, 0.1f));
					OverlapUtility();
				}
			}
		}
	}
}

void AProjectile::OverlapUtility()
{
	if (OverlapParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
	}
	if (OverlapSound)
	{
		UGameplayStatics::PlaySound2D(this, OverlapSound);
	}
	if (bDestroyOnHit)
	{
		Destroy();
	}
}