// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HP = 30.f;
	KnockBack = 500.f;
	Damage = 15.f;
	MovementSpeed = 450.f;
	TurnRate = 250.f;

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
	HP -= DamageValue; // Deals Damage
	LaunchCharacter(Direction * KnockBackForce, true, true); // launches enemy to make the player feel the force from the hit
	if (HP <= 0.f)
	{
		Destroy(); // Destroy the enemy if it looses all HP
	}
}
