// Fill out your copyright notice in the Description page of Project Settings.


#include "SafeDrop.h"
#include "Main.h"


ASafeDrop::ASafeDrop()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASafeDrop::BeginPlay()
{
	Super::BeginPlay();

}

void ASafeDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASafeDrop::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->LastSafeDrop = GetActorLocation() + FVector(0.f, 0.f, 1100.f);
		}
	}
}

void ASafeDrop::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}