// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "Main.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"


ABoss::ABoss()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(GetRootComponent());
	AggroSphere->InitSphereRadius(1000.f);

	CombatTarget = nullptr;

	bOverlappingAggrosphere = false;

}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	SetBossMovementStatus(EBossMovementStatus::EMS_Idle);

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::AggroSphereOnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::AggroSphereOnOverlapEnd);

}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoss::AggroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (!CombatTarget)
			{
				CombatTarget = Main;
				TeleportBehindCombatTarget();
				SetBossMovementStatus(EBossMovementStatus::EMS_Teleport);
			}
			else
			{
				SetBossMovementStatus(EBossMovementStatus::EMS_MidRange);
			}
			bOverlappingAggrosphere = true;
		}
	}
}

void ABoss::AggroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			bOverlappingAggrosphere = false;
			TeleportBehindCombatTarget();
			SetBossMovementStatus(EBossMovementStatus::EMS_Teleport);

		}
	}
}

void ABoss::TeleportBehindCombatTarget()
{
	FVector CurentPosition = GetActorLocation();
	FVector CombatTargetLocation = CombatTarget->GetActorLocation();

	FRotator ToCombatTargetRotation = UKismetMathLibrary::FindLookAtRotation(CurentPosition, CombatTargetLocation);
	FRotator YawToCombatTargetRotation = FRotator(0.f, ToCombatTargetRotation.Yaw, 0.f);
	// get forward vector
	FVector Direction = FRotationMatrix(YawToCombatTargetRotation).GetUnitAxis(EAxis::X);

	SetActorLocation(CombatTargetLocation + (Direction * 500.f));
	SetActorRotation(YawToCombatTargetRotation + FRotator(0.f, 180.f, 0.f));
}