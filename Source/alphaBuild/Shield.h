// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Shield.generated.h"

UENUM(BluePrintType)
enum class EShieldState :uint8
{
	EWS_Pickup		UMETA(DisplayName = "Pickup"),
	EWS_Equipped	UMETA(DisplayName = "Equipped"),

	EWS_MAX			UMETA(DisplayName = "DefaultMax")
};

/**
 * 
 */
UCLASS()
class ALPHABUILD_API AShield : public AItem
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:

	AShield();

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Item")
	EShieldState ShieldState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Item | Particles")
	bool bWeaponParticles;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
		class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
		float KnockBack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
		class AMain* EquippedOn;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void Equip(class AMain* Char);

	FORCEINLINE void SetShieldState(EShieldState State) { ShieldState = State; }
	FORCEINLINE EShieldState GetShieldState() { return ShieldState; }

	UFUNCTION()
		void ShieldCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void ShieldCombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void ActivateCollision();

	UFUNCTION(BlueprintCallable)
		void DeactivateCollision();
	
};
