// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HP = 70.f;
	KnockBack = 500.f;
	Damage = 15.f;
	MovementSpeed = 450.f;
	TurnRate = 250.f;

	bBlocked = false;
	bCanTakeDamage = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Configure character movement
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character faces in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, TurnRate, 0.0f); // ...at ths rotation rate
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed; // Sets Movement Speed
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::TakeDMG(float DamageValue, float KnockBackForce, FVector Direction)
{
	if (bCanTakeDamage)
	{
		HP -= DamageValue; // Deals Damage
		LaunchCharacter(Direction * KnockBackForce, true, true); // launches enemy to make the player feel the force from the hit
		bCanTakeDamage = false;
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->BrakingFrictionFactor = 0.f; // Removes friction
		GetWorldTimerManager().SetTimer(DMGHandle, this, &AEnemy::EnableTakeDMG, 0.1f);
	}
	if (HP <= 0.f)
	{
		GetCharacterMovement()->StopMovementImmediately();
		//Destroy(); // Destroy the enemy if it looses all HP
	}
}

void AEnemy::EnableTakeDMG()
{
	bCanTakeDamage = true;
	GetCharacterMovement()->BrakingFrictionFactor = 2.f; // Sets friction back to default
	GetWorldTimerManager().ClearTimer(DMGHandle);
}

void AEnemy::Die()
{
	if (DeathParticlesComponent)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticlesComponent, GetActorLocation(), FRotator(0.f), true);
	}
	Destroy();
}