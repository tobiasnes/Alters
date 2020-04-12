// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UCLASS()
class ALPHABUILD_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCamera;

	// Basic player stats
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float HP;

	// Base movement values
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeedDash;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeedFury;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeedDefence;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeedRanged;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float TurnRate{ 540.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	int StyleIndex;

    float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	// ability unlocked?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Styles)
	bool bDashKnifeUnlocked{ false };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Styles)
	bool bFuryUnlocked{ false };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Styles)
	bool bDefenceUnlocked{ false };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Styles)
	bool bRangedUnlocked{ false };

	// Ability values
	// Dash values
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	float DashDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	float DashCooldown;
	UPROPERTY()
	bool bCanDash;
	UPROPERTY()
	bool bDashing;
	UPROPERTY()
	FVector SpeedBeforeDash;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	float DashStop;
	UPROPERTY()
	FTimerHandle DashHandle;

	//SpawnFuryWeapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		TSubclassOf<class AWeapon> SpawnerClass;

	//SpawnShield
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		TSubclassOf<class AShield> ShieldSpawnerClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called for forward and backward input
	void MoveUp(float Value);

	// Called for left and right input
	void MoveRight(float Value);

	// Style Swapping
	void DashStyle();
	void FuryStyle();
	void DefenseStyle();
	void RangedStyle();

	// Style moves
	void Move1Pressed();
	void Move1Released();
	void Move2Pressed();
	void Move2Released();

	bool bMove1Pressed;
	bool bMove2Pressed;

	//Weapon Equips
	bool bEquipPressed;
	bool bWeaponEquipped;

	bool bShieldEquipped;

	bool bDashAttack;

	bool bFuryAttack1;
	bool bFuryAttack2;

	void EquipPressed();
	void EquipReleased();

	//UFUNCTION(BlueprintCallable)
	//void EquipMesh();

	// Dash functions
	UFUNCTION()
	void Dash();
	UFUNCTION()
	void StopDashing();
	UFUNCTION()
	void ResetDash();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AShield* EquippedShield;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	void SetEquippedShield(AShield* ShieldToSet);
	FORCEINLINE AShield* GetEquippedShield() { return EquippedShield; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bBlocking;

	void Attack();
	void Attack2();
	void Attack4();
	void BlockStart();
	void BlockEnd();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* AlterMontage;

	void TakeDMG(float DamageValue, float KnockBackForce, FVector Direction);

	UFUNCTION(BlueprintCallable)
	void Die();

};
