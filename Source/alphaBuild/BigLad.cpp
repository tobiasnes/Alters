// Fill out your copyright notice in the Description page of Project Settings.


#include "BigLad.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/Decalcomponent.h"
#include "AIController.h"
#include "AIModule.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"


ABigLad::ABigLad()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(GetRootComponent());
	AggroSphere->InitSphereRadius(1000.f);

	WalkSphere = CreateDefaultSubobject<USphereComponent>(TEXT("WalkSphere"));
	WalkSphere->SetupAttachment(GetRootComponent());
	WalkSphere->InitSphereRadius(450.f);

	CombatSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(165.f);

	AttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackBox"));
	AttackBox->SetupAttachment(GetRootComponent());

	ChargePath = CreateDefaultSubobject<UDecalComponent>("ChargePath");
	ChargePath->SetupAttachment(RootComponent);

	bOverlappingCombatSphere = false;
	bOverlappingWalkSphere = false;
	MainInHitRange = false;

	InterpSpeed = 5.f;
	bInterpToMain = false;

	bIsLockingOn = false;
	bIsCharging = false;
	bIsExhausted = false;
	LockOnTime = 2.f;
	ChargeSpeed = 1000.f;
	ChargeTime = 2.f;
	ExhaustedTime = 0.5f;
	ChargeDamage = 45.f;

}

void ABigLad::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABigLad::AggroSphereOnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &ABigLad::AggroSphereOnOverlapEnd);

	WalkSphere->OnComponentBeginOverlap.AddDynamic(this, &ABigLad::WalkSphereOnOverlapBegin);
	WalkSphere->OnComponentEndOverlap.AddDynamic(this, &ABigLad::WalkSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ABigLad::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ABigLad::CombatSphereOnOverlapEnd);

	AttackBox->OnComponentBeginOverlap.AddDynamic(this, &ABigLad::AttackBoxOnOverlapBegin);
	AttackBox->OnComponentEndOverlap.AddDynamic(this, &ABigLad::AttackBoxOnOverlapEnd);

	ChargePath->SetHiddenInGame(true);

}

// Called every frame
void ABigLad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToMain && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (BigLadMovementStatus == EBigLadMovementStatus::EMS_LockOnTarget && (!bIsLockingOn) && (!bIsCharging) && (!bIsExhausted))
	{
		UE_LOG(LogTemp, Warning, TEXT("lockon established"));
		bIsLockingOn = true;
		SetInterpToMain(true);
		ChargePath->SetHiddenInGame(false);
		GetWorldTimerManager().SetTimer(ChargeHandle, this, &ABigLad::StartCharging, LockOnTime);
	}

	if ((BigLadMovementStatus == EBigLadMovementStatus::EMS_ChargeToTarget) && (!bIsCharging) && (!bIsExhausted))
	{
		UE_LOG(LogTemp, Warning, TEXT("activate charge"));
		bIsCharging = true;
		SetInterpToMain(false);
		ChargePath->SetHiddenInGame(true);
		// Set Timer to stop Charge
		GetWorldTimerManager().SetTimer(ChargeHandle, this, &ABigLad::StartResting, ChargeTime);
	}

	if (bIsCharging)
	{
		FVector Movement = GetActorForwardVector() * DeltaTime * ChargeSpeed;

		SetActorLocation(GetActorLocation() + Movement);
	}

	if (!bCanTakeDamage)
	{
		if (HP > 0.f)
		{
			GetWorldTimerManager().SetTimer(MoveHandle, this, &ABigLad::ContinnueChase, 0.1f);
		}
	}
}

FRotator ABigLad::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator YawLookAtRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
	return YawLookAtRotation;
}

void ABigLad::SetInterpToMain(bool interp)
{
	bInterpToMain = interp;
}

void ABigLad::StartCharging()
{
	UE_LOG(LogTemp, Warning, TEXT("state is charge"));
	bIsLockingOn = false;
	SetBigLadMovementStatus(EBigLadMovementStatus::EMS_ChargeToTarget);
}

void ABigLad::StartResting()
{
	UE_LOG(LogTemp, Warning, TEXT("charge over"));
	bIsCharging = false;
	bIsExhausted = true;
	//InterpSpeed = 1.f;
	//SetInterpToMain(true);
	GetWorldTimerManager().SetTimer(ChargeHandle, this, &ABigLad::StopResting, ExhaustedTime);
}

void ABigLad::StopResting()
{
	UE_LOG(LogTemp, Warning, TEXT("ready to go choosing action"));
	bIsExhausted = false;
	InterpSpeed = 5.f;
	if (bOverlappingCombatSphere)
	{
		UE_LOG(LogTemp, Warning, TEXT("action slap"));
		SetInterpToMain(true);
		SetBigLadMovementStatus(EBigLadMovementStatus::EMS_Attacking);
	}
	else if (bOverlappingWalkSphere)
	{
		UE_LOG(LogTemp, Warning, TEXT("action move"));
		SetInterpToMain(false);
		MoveToTarget(CombatTarget);
		SetBigLadMovementStatus(EBigLadMovementStatus::EMS_MoveToTarget);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("action repeat"));
		SetInterpToMain(true);
		SetBigLadMovementStatus(EBigLadMovementStatus::EMS_LockOnTarget);
	}
}

void ABigLad::AggroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				CombatTarget = Main;
				SetInterpToMain(true);
				SetBigLadMovementStatus(EBigLadMovementStatus::EMS_LockOnTarget);
			}
		}
	}
}

void ABigLad::AggroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				// removed for now
				//CombatTarget = nullptr;
				//SetInterpToMain(false);
				//SetBigLadMovementStatus(EBigLadMovementStatus::EMS_Idle);
			}
		}
	}
}

void ABigLad::WalkSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bOverlappingWalkSphere = true;
				if (BigLadMovementStatus != EBigLadMovementStatus::EMS_LockOnTarget && (!bIsCharging) && (!bIsExhausted))
				{
					SetInterpToMain(false);
					MoveToTarget(Main);
					SetBigLadMovementStatus(EBigLadMovementStatus::EMS_MoveToTarget);
				}
			}
		}
	}
}

void ABigLad::WalkSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bOverlappingWalkSphere = false;
				if (BigLadMovementStatus != EBigLadMovementStatus::EMS_LockOnTarget && (!bIsCharging) && (!bIsExhausted))
				{
					SetInterpToMain(true);
					AIController->StopMovement();
					SetBigLadMovementStatus(EBigLadMovementStatus::EMS_LockOnTarget);
				}
			}
		}
	}
}

void ABigLad::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				Main->SetCombatTarget(this);
				bOverlappingCombatSphere = true;
				if ((!bIsLockingOn) && (!bIsCharging) && (!bIsExhausted))
				{
					SetInterpToMain(true);
					SetBigLadMovementStatus(EBigLadMovementStatus::EMS_Attacking);
				}	
			}
		}
	}
}

void ABigLad::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				Main->SetCombatTarget(nullptr);
				bOverlappingCombatSphere = false;
				if ((!bIsLockingOn) && (!bIsCharging) && (!bIsExhausted))
				{
					SetInterpToMain(false);
					if (BigLadMovementStatus != EBigLadMovementStatus::EMS_Attacking)
					{
						MoveToTarget(Main);
					}
				}
			}
		}
	}
}

void ABigLad::MoveToTarget(class AMain* Target)
{
	SetBigLadMovementStatus(EBigLadMovementStatus::EMS_MoveToTarget);

	if (AIController)
	{
		FAIMoveRequest MoveRequest; // Struct containing the target
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(25.0f); // minmum distance between EnemyHitBox and TargetHitBox before the enemy stops moving

		FNavPathSharedPtr NavPath; // Will store navigatin data for the Enemy AI to use

		AIController->MoveTo(MoveRequest, &NavPath); // Makes the Enemy follow player until it gets close enough to attack

		/**
		auto PathPoints = NavPath->GetPathPoints();
		for (auto Point : PathPoints)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 1.5f);
		}
		*/
	}
}

void ABigLad::AttackBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				MainInHitRange = true;
				if (bIsCharging)
				{
					HitPlayer(ChargeDamage);
					GetWorld()->GetTimerManager().ClearTimer(ChargeHandle);
					StartResting();
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("AttackBoxOnOverlapBegin()"));
}

void ABigLad::AttackBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				MainInHitRange = false;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AttackBoxOnOverlapEnd()"));
}

void ABigLad::HitPlayer(float DMG)
{
	if (MainInHitRange)
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

void ABigLad::ContinnueChase()
{
	if (CombatTarget && !bIsCharging && !bIsExhausted)
	{
		MoveToTarget(CombatTarget);
	}
}