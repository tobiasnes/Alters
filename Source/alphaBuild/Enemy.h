// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class ALPHABUILD_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// Movement values
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float TurnRate;

	// Stats
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float KnockBack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float Damage;

	UPROPERTY()
	bool bBlocked;

	FTimerHandle DMGHandle;
	bool bCanTakeDamage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TakeDMG(float DamageValue, float KnockBackForce, FVector Direction);

	void EnableTakeDMG();

};
