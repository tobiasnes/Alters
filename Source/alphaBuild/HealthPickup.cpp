// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "Main.h"


AHealthPickup::AHealthPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	HealAmmount = 30.f;
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();

}

void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealthPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->HP += HealAmmount;
			if (Main->HP > 100.f)
			{
				Main->HP = 100.f;
			}
			Destroy();
		}
	}
}

void AHealthPickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}