// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "StageHazard.generated.h"

/**
 * 
 */
UCLASS()
class ALPHABUILD_API AStageHazard : public AItem
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AStageHazard();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | StageHazard")
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | StageHazard")
	float KnockBack;

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
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
