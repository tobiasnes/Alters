// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Boss.generated.h"

UENUM(BlueprintType)
enum class EBossMovementStatus :uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_Teleport		UMETA(DisplayName = "Teleport"),
	EMS_Charge			UMETA(DisplayName = "Charge"),
	EMS_Walk			UMETA(DisplayName = "Walk"),
	EMS_FireBreath		UMETA(DisplayName = "FireBreath"),
	EMS_Melee			UMETA(DisplayName = "Melee"),
	EMS_Exhausted		UMETA(DisplayName = "Exhauseted"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};


/**
 * 
 */
UCLASS()
class ALPHABUILD_API ABoss : public AEnemy
{
	GENERATED_BODY()

public:

	ABoss();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EBossMovementStatus BossMovementStatus;

	UFUNCTION(BlueprintCallable)
	void SetBossMovementStatus(EBossMovementStatus Status);

	// Sphere that will aggro the ai if the player overlaps
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AggroSphere;

	// Sphere that will make ai attack  if the player overlaps
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* CombatSphere;

	// The target the boss will attack
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AMain* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bOverlappingAggroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bOverlappingCombatSphere;

	// Will be used as hitbox for the charge attack
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	class UBoxComponent* ChargeBox;

	// Values for the move where the AI charges the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float ChargeSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float ChargeTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float ChargeDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float ExhaustedTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsCharging;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bIsExhausted;
	FTimerHandle ChargeHandle;

	// Everything interping
	float InterpSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bInterpToMain;
	FRotator GetLookAtRotationYaw(FVector Target);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<class AProjectile> FireSpawnerClass;

	// Firebreath values
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireBreath")
	float FireDelay;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float TimeSinceLastShot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float Walktimer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FVector MoveDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bIsBreathingFire;

	// Melee values
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
	float MeleeDamage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void AggroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AggroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void ChargeBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void ChargeBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void TeleportBehindCombatTarget();

	// After Charge
	UFUNCTION()
	void StartResting();
	UFUNCTION()
	void StopResting();

	// Fire Breath
	UFUNCTION(BlueprintCallable)
	void StartFireBreath();
	UFUNCTION(BlueprintCallable)
	void StopFireBreath();

	UFUNCTION(BlueprintCallable)
	void HitPlayer(float DMG);

	// Reminds the AI, where the player is and what to do
	UFUNCTION(BlueprintCallable)
	void RefreshAfterAttack();

	UFUNCTION(BlueprintCallable)
	void JustStop();

};
