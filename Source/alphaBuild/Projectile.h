// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class ALPHABUILD_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	class USphereComponent* HitBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	class UParticleSystemComponent* IdleParticlesComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	class UParticleSystem* OverlapParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	class USoundCue* OverlapSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Projectile")
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Projectile")
	float KnockBack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Projectile")
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Projectile")
	float MaxLifeSpan;
	UPROPERTY()
	float CurrentLifeSpan;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Projectile")
	bool bDestroyOnHit;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Projectile")
	bool bHarmsMain;

	bool bBlocked;

	UPROPERTY()
	FVector ForwardVector;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapUtility();

};
