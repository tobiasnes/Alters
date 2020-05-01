// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Cat.generated.h"

/**
 * 
 */
UCLASS()
class ALPHABUILD_API ACat : public AItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACat();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Saved Player Values")
	float SavedMaxWalkSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Saved Player Values")
	FRotator SavedRotationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Cat")
	class AMain* Target;

	float Time;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	FRotator GetLookAtRotationYaw(FVector TargetLocation);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
