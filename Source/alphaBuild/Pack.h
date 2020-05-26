// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Pack.generated.h"

UENUM(BlueprintType)
enum class EPackMovementStatus :uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking		UMETA(DisplayName = "Attacking"),
	EMS_Stunned			UMETA(DisplayName = "Stunned"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};


/**
 * 
 */
UCLASS()
class ALPHABUILD_API APack : public AEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APack();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EPackMovementStatus PackMovementStatus;

	FORCEINLINE void SetSmallEnemyMovementStatus(EPackMovementStatus Status) { PackMovementStatus = Status; }

	// Sphere that will aggro the ai if the player overlaps
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AggroSphere;

	// Sphere that will make ai attack  if the player overlaps
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

	bool bStunned;
	FTimerHandle StunHandle;
	FTimerHandle MoveHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void AggroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AggroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// tells the AI if the player is within aggro range
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingAggroSphere;

	// Move to target (UE4 is retarded and stops the movement for no reason when i apply stun)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AMain* MoveTo;

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMain* Target);

	virtual void TakeDMG(float DamageValue, float KnockBackForce, FVector Direction);

	// tells the AI if the player is within mele range
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	// Attack Target (atm for use in the animasion BP)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AMain* CombatTarget;

	// tells the AI if the player is within mele range of the hit
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool MainInHitRange;

	UFUNCTION()
	virtual void AttackBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AttackBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void HitPlayer();

	UFUNCTION(BlueprintCallable)
	void StunnStart(float Time);
	UFUNCTION()
	void StunnEnd();

	void ContinnueChase();

};
