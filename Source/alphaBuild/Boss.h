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
	EMS_MidRange		UMETA(DisplayName = "MidRange"),
	EMS_Melee			UMETA(DisplayName = "Melee"),

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

	FORCEINLINE void SetBossMovementStatus(EBossMovementStatus Status) { BossMovementStatus = Status; }

	// Sphere that will aggro the ai if the player overlaps
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AggroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AMain* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bOverlappingAggrosphere;

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

	void TeleportBehindCombatTarget();

};
