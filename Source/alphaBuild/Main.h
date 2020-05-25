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
	UPROPERTY()
	bool bZoom;
	UPROPERTY()
	bool bReverseZoom;
	UPROPERTY()
	float ZoomTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Static Mesh")
	class UStaticMeshComponent* DreamCatcherMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Decal", meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* AimArrow;

	// Basic player stats
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	bool Frozen;

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
	FVector LastSafeDrop; // The last safe location to drop the player if he where to fall off the map

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	int StyleIndex;

    float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	// ability unlocked?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Styles)
	bool bDashKnifeUnlocked{ false };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Styles)
	bool bFuryUnlocked{ false };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Styles)
	bool bDefenceUnlocked{ false };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Styles)
	bool bRangedUnlocked{ false };

	//ability widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Styles)
		bool bFuryWidget{ false };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Styles)
		bool bDefenceWidget{ false };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Styles)
		bool bRangedWidget{ false };
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

	//Spawn Bow
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		TSubclassOf<class ABow> BowSpawnerClass;

	//void SaveGame();

	//void LoadGame();

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

	FName CurrentLevelName;

	//Weapon Equips
	bool bEquipPressed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unlocks")
	bool bWeaponEquipped;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unlocks")
	bool bShieldEquipped;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unlocks")
	bool bBowEquipped;

	bool bDashAttack;

	bool bFuryAttack1;
	bool bFuryAttack2;

	void EquipPressed();
	void EquipReleased();

	public:

	//UFUNCTION(BlueprintCallable)
	//void EquipMesh();

	// Dash functions
	UFUNCTION()
	void Dash();
	UFUNCTION()
	void StopDashing();
	UFUNCTION()
	void ResetDash();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	class AShield* EquippedShield;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	class ABow* EquippedBow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	void SetEquippedShield(AShield* ShieldToSet);
	FORCEINLINE AShield* GetEquippedShield() { return EquippedShield; }

	void SetEquippedBow(ABow* BowToSet);
	FORCEINLINE ABow* GetEquippedBow() { return EquippedBow; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bBlocking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsInDashStyle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsInRangedStyle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAiming;

	void Attack();
	void Attack2();
	void Attack4();
	void BlockStart();
	void BlockEnd();
	void ShieldStun();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* AlterMontage;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		//class UAnimMontage* DeathMontage;

	void TakeDMG(float DamageValue, float KnockBackForce, FVector Direction);

	UFUNCTION(BlueprintCallable)
	void Freeze();

	UFUNCTION()
	void UnFreeze(float SavedMaxWalkSpeed, FRotator SavedRotationRate);

	UFUNCTION()
	void SetCameraDistance(float Distance);

};
