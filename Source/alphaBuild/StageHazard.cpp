// Fill out your copyright notice in the Description page of Project Settings.


#include "StageHazard.h"
#include "Kismet/KismetMathLibrary.h"
#include "Main.h"

AStageHazard::AStageHazard()
{
	PrimaryActorTick.bCanEverTick = true;

	Damage = 10.f;
	KnockBack = 500.f;
}

void AStageHazard::BeginPlay()
{
	Super::BeginPlay();

}

void AStageHazard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStageHazard::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			FRotator ToTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Main->GetActorLocation());
			FRotator YawToTargetRotation = FRotator(0.f, ToTargetRotation.Yaw, 0.f);
			// get forward vector
			FVector Direction = FRotationMatrix(YawToTargetRotation).GetUnitAxis(EAxis::X);
			Main->TakeDMG(Damage, KnockBack, Direction);
		}
	}
}

void AStageHazard::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

