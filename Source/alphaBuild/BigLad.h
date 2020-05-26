// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "BigLad.generated.h"

UENUM(BlueprintType)
enum class EBigLadMovementStatus :uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_ChargeToTarget	UMETA(DisplayName = "ChargetoTarget"),
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking		UMETA(DisplayName = "Attacking"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};


/**
 * 
 */
UCLASS()
class ALPHABUILD_API ABigLad : public AEnemy
{
	GENERATED_BODY()
	
public:

	ABigLad();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EBigLadMovementStatus BigLadMovementStatus;

	FORCEINLINE void SetBigLadMovementStatus(EBigLadMovementStatus Status) { BigLadMovementStatus = Status; }

	// Sphere that will aggro the ai if the player overlaps
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AggroSphere;

	// Sphere that will make the enemy wal towards te player instead of tryint to charge him if he stops one of his charge attacks inside it.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* WalkSphere;

	// Sphere that will make ai attack if the player overlaps
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	// The hitbox of the attack
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	class UBoxComponent* AttackBox;

	float InterpSpeed;
	bool bInterpToMain;
	void SetInterpToMain(bool interp);
	FRotator GetLookAtRotationYaw(FVector Target);

	// Values for the move where the AI charges the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float ChargeSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float ChargeTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float ChargeDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float ExhaustedTime;
	FTimerHandle ChargeHandle;

	FTimerHandle MoveHandle;

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

	// tells the AI if it is charging the enemy
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bIsCharging;

	// tells the AI if it is exhausetd
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bIsExhausted;

	// After Charge
	UFUNCTION()
	void StartResting();
	UFUNCTION()
	void StopResting();

	UFUNCTION()
	virtual void WalkSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void WalkSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMain* Target);

	// tells the AI if the player is within mele range
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	// tells the AI if the player is to close to Charge at him
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingWalkSphere;

	// Attack Target (atm for use in the animasion BP)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class AMain* CombatTarget;

	// tells the AI if the player is within mele range of the hit
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool MainInHitRange;

	UFUNCTION()
	virtual void AttackBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AttackBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void HitPlayer(float DMG);

	void ContinnueChase();

};
