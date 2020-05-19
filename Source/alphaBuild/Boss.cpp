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

	CombatSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(200.f);

	CombatTarget = nullptr;

	bOverlappingAggroSphere = false;
	bOverlappingCombatSphere = false;

	ChargeSpeed = 1000.f;
	ChargeTime = 1.f;
	ChargeDamage = 35.f;
	ExhaustedTime = 2.5f;
	bIsCharging = false;
	bIsExhausted = false;

}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	SetBossMovementStatus(EBossMovementStatus::EMS_Idle);

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::AggroSphereOnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::AggroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::CombatSphereOnOverlapEnd);

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
			bOverlappingAggroSphere = true;
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
			bOverlappingAggroSphere = false;
			TeleportBehindCombatTarget();
			SetBossMovementStatus(EBossMovementStatus::EMS_Teleport);
		}
	}
}

void ABoss::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bOverlappingCombatSphere = true;
			}
		}
	}
}

void ABoss::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bOverlappingCombatSphere = false;
			}
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

void ABoss::HitPlayer(float DMG)
{
	if (bOverlappingCombatSphere)
	{
		// cast TakeDMG function on player
		if (CombatTarget)
		{
			USceneComponent* SceneComp = Cast<USceneComponent>(GetComponentByClass(USceneComponent::StaticClass()));
			const FRotator Rotation = SceneComp->GetComponentRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			// get forward vector
			FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			Direction.Z = 0.8f;

			float x = Direction.X;
			float y = Direction.Y;
			float z = Direction.Z;

			if (bBlocked && !bIsCharging)
			{
				DMG = 0.f;
				KnockBack = 0.f;
				Direction = FVector(0.f);
			}

			UE_LOG(LogTemp, Warning, TEXT("x: %f y: %f z: %f"), x, y, z);
			Cast<AMain>(CombatTarget)->TakeDMG(DMG, KnockBack, Direction);
		}
		UE_LOG(LogTemp, Warning, TEXT("Player Got Hit!"));
	}
	UE_LOG(LogTemp, Warning, TEXT("HitPlayer()"));
}