// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Lever.generated.h"

/**
 * 
 */
UCLASS()
class ALPHABUILD_API ALever : public AItem
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ALever();

	// Mesh for the Lever
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lever")
	class UStaticMeshComponent* LeverMesh;

	// Mesh for the floating platform
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* PlatformMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class UParticleSystem* ArrowOverlapParticles;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector StartPoint;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	bool bInterping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lever")
	bool bLeverMoves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lever")
	float RotationSpeed;

	float DegreesRotated;
	
	float Distance;

	FVector CurrentLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lever")
	bool bLeverActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lever")
	bool bActivatesOnOverlap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void PullLever();

};
