// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Bow.generated.h"

UENUM(BluePrintType)
enum class EBowState :uint8
{
	EBS_Pickup		UMETA(DisplayName = "Pickup"),
	EBS_Equipped	UMETA(DisplayName = "Equipped"),

	EBS_MAX			UMETA(DisplayName = "DefaultMax")
};

/**
 * 
 */
UCLASS()
class ALPHABUILD_API ABow : public AItem
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:

	ABow();

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Item")
		EBowState BowState;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Item | Particles")
		bool bBowParticles;

	bool bCanShoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
		class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Item | Sound")
		class USoundCue* OnEquipSound;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
		class AMain* EquippedOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		TSubclassOf<class AProjectile> ArrowSpawnerClass;

	UPROPERTY()
	FTimerHandle ShootHandle;

	void SpawnArrow();

	void CanShootAgain();

	float SpawnTimer;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
		virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void Equip(class AMain* Char);

	FORCEINLINE void SetBowState(EBowState State) { BowState = State; }
	FORCEINLINE EBowState GetBowState() { return BowState; }
};
