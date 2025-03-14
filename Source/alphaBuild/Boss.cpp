// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "Main.h"
#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/World.h"


ABoss::ABoss()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(GetRootComponent());
	AggroSphere->InitSphereRadius(1000.f);

	CombatSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(250.f);

	ChargeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ChargeBox"));
	ChargeBox->SetupAttachment(GetRootComponent());

	CombatTarget = nullptr;

	bOverlappingAggroSphere = false;
	bOverlappingCombatSphere = false;

	ChargeSpeed = 1200.f;
	ChargeTime = 1.f;
	ChargeDamage = 35.f;
	ExhaustedTime = 2.5f;
	bIsCharging = false;
	bIsExhausted = false;

	FireDelay = 0.1f;
	TimeSinceLastShot = 0.1f;
	Walktimer = 0.f;
	bIsBreathingFire = false;

	MeleeDamage = 35.f;

}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	SetBossMovementStatus(EBossMovementStatus::EMS_Idle);

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::AggroSphereOnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::AggroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::CombatSphereOnOverlapEnd);

	ChargeBox->OnComponentBeginOverlap.AddDynamic(this, &ABoss::ChargeBoxOnOverlapBegin);
	ChargeBox->OnComponentEndOverlap.AddDynamic(this, &ABoss::ChargeBoxOnOverlapEnd);

}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (GetActorLocation().Z < -400.f)
	{
		SetBossMovementStatus(EBossMovementStatus::EMS_Teleport);
		GetWorld()->GetTimerManager().ClearTimer(ChargeHandle);
		bIsCharging = false;
		bIsExhausted = false;
		InterpSpeed = 2.5f;
		bInterpToMain = true;
	}

	if (bInterpToMain && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if ((BossMovementStatus == EBossMovementStatus::EMS_Charge) && (!bIsCharging) && (!bIsExhausted))
	{
		bIsCharging = true;
		InterpSpeed = 1.f;
		// Set Timer to stop Charge
		GetWorldTimerManager().SetTimer(ChargeHandle, this, &ABoss::StartResting, ChargeTime);
	}

	if (bIsCharging)
	{
		FVector Movement = GetActorForwardVector() * DeltaTime * ChargeSpeed;
		UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), Movement.X, Movement.Y, Movement.Z);
		SetActorLocation(GetActorLocation() + Movement);
	}

	if (BossMovementStatus == EBossMovementStatus::EMS_Walk)
	{
		Walktimer += DeltaTime;
		MoveDirection = GetActorForwardVector() * DeltaTime * MovementSpeed;
		SetActorLocation(GetActorLocation() + MoveDirection);
		if (Walktimer >= 1.5f)
		{
			SetBossMovementStatus(EBossMovementStatus::EMS_FireBreath);
		}
	}

	if (bIsBreathingFire)
	{
		if (TimeSinceLastShot >= FireDelay)
		{
			if (FireSpawnerClass)
			{
				FVector Location = GetMesh()->GetSocketLocation("Head_Socket");
				FTransform FireSpawnTransform;
				FireSpawnTransform.SetLocation(Location);
				FRotator Rotation = FRotator::ZeroRotator;
				FVector trash;
				GetMesh()->GetSocketWorldLocationAndRotation("Head_Socket", trash, Rotation);
				// UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), Rotation.Pitch, Rotation.Yaw, Rotation.Roll);
				FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);
				FireSpawnTransform.SetRotation(YawRotation.Quaternion());
				GetWorld()->SpawnActor<AProjectile>(FireSpawnerClass, FireSpawnTransform);
			}
			TimeSinceLastShot = 0.f;
		}
		TimeSinceLastShot += DeltaTime;
	}
}

void ABoss::StartResting()
{
	bIsCharging = false;
	bIsExhausted = true;
	bInterpToMain = false;
	SetBossMovementStatus(EBossMovementStatus::EMS_Exhausted);
	GetWorldTimerManager().SetTimer(ChargeHandle, this, &ABoss::StopResting, ExhaustedTime);
}

void ABoss::StopResting()
{
	bIsExhausted = false;
	InterpSpeed = 2.5f;
	bInterpToMain = true;

	//Set stuff in acordance to CombatTarget location
	RefreshAfterAttack();
}

FRotator ABoss::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator YawLookAtRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
	return YawLookAtRotation;
}

void ABoss::AggroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && !bIsCharging && !bIsExhausted)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (!CombatTarget)
			{
				CombatTarget = Main;
				SetBossMovementStatus(EBossMovementStatus::EMS_Teleport);
				bInterpToMain = true;
			}
			else if (!bIsCharging && !bIsExhausted && BossMovementStatus != EBossMovementStatus::EMS_FireBreath)
			{
				//not sure if I'll need this "else if"
			}
			bOverlappingAggroSphere = true;
		}
	}
}

void ABoss::AggroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && !bIsCharging && !bIsExhausted)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			bOverlappingAggroSphere = false;
			if (!bIsCharging && !bIsExhausted && BossMovementStatus != EBossMovementStatus::EMS_FireBreath)
			{
				SetBossMovementStatus(EBossMovementStatus::EMS_Teleport);
				UE_LOG(LogTemp, Warning, TEXT("DO IT!"));
			}
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
				if (!bIsCharging && !bIsExhausted && BossMovementStatus != EBossMovementStatus::EMS_FireBreath)
				{
					SetBossMovementStatus(EBossMovementStatus::EMS_Melee);
				}
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
				if (!bIsCharging && !bIsExhausted && BossMovementStatus != EBossMovementStatus::EMS_FireBreath)
				{
					// Not sure if I'll be neding this as well
				}
			}
		}
	}
}

void ABoss::StartFireBreath()
{
	bIsBreathingFire = true;
}

void ABoss::StopFireBreath()
{
	bIsBreathingFire = false;

	//Set stuff in acordance to CombatTarget location
	RefreshAfterAttack();
}

void ABoss::ChargeBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsCharging && OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			HitPlayer(ChargeDamage);
			StartResting();
		}
	}
}

void ABoss::ChargeBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Nothing lol
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

			UE_LOG(LogTemp, Warning, TEXT("x: %f y: %f z: %f"), x, y, z);
			Cast<AMain>(CombatTarget)->TakeDMG(DMG, KnockBack, Direction);
		}
		UE_LOG(LogTemp, Warning, TEXT("Player Got Hit!"));
	}
	UE_LOG(LogTemp, Warning, TEXT("HitPlayer()"));
}

void ABoss::RefreshAfterAttack()
{
	if (bOverlappingCombatSphere)
	{
		SetBossMovementStatus(EBossMovementStatus::EMS_Melee);
	}
	else if (bOverlappingAggroSphere)
	{
		Walktimer = 0.f;
		SetBossMovementStatus(EBossMovementStatus::EMS_Walk);
	}
	else
	{
		SetBossMovementStatus(EBossMovementStatus::EMS_Teleport);
	}
}

void ABoss::SetBossMovementStatus(EBossMovementStatus Status)
{ 
	BossMovementStatus = Status;
}

void ABoss::JustStop()
{
	BossMovementStatus = EBossMovementStatus::EMS_Idle;
	bIsBreathingFire = false;
	bIsCharging = false;
	bIsExhausted = false;
	bInterpToMain = false;
	MovementSpeed = 0.f;
}