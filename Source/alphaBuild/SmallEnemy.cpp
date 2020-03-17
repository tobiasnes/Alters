// Fill out your copyright notice in the Description page of Project Settings.


#include "SmallEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "AIController.h"
#include "AIModule.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASmallEnemy::ASmallEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(GetRootComponent());
	AggroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	AttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackBox"));
	AttackBox->SetupAttachment(GetRootComponent());

	bOverlappingCombatSphere = false;
	MainInHitRange = false;

	HP = 30.f;
	MovementSpeed = 450.f;
	TurnRate = 250.f;

}

// Called when the game starts or when spawned
void ASmallEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &ASmallEnemy::AggroSphereOnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &ASmallEnemy::AggroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ASmallEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ASmallEnemy::CombatSphereOnOverlapEnd);

	AttackBox->OnComponentBeginOverlap.AddDynamic(this, &ASmallEnemy::AttackBoxOnOverlapBegin);
	AttackBox->OnComponentEndOverlap.AddDynamic(this, &ASmallEnemy::AttackBoxOnOverlapEnd);

}

// Called every frame
void ASmallEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASmallEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASmallEnemy::AggroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			MoveToTarget(Main);
		}
	}
}

void ASmallEnemy::AggroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				SetSmallEnemyMovementStatus(ESmallEnemyMovementStatus::EMS_Idle);
				if (AIController)
				{
					AIController->StopMovement();
				}
			}
		}
	}
}

void ASmallEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				CombatTarget = Main;
				bOverlappingCombatSphere = true;
				SetSmallEnemyMovementStatus(ESmallEnemyMovementStatus::EMS_Attacking);
			}
		}
	}
}

void ASmallEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bOverlappingCombatSphere = false;
				if (SmallEnemyMovementStatus != ESmallEnemyMovementStatus::EMS_Attacking)
				{
					MoveToTarget(Main);
					CombatTarget = nullptr;
				}
			}
		}
	}
}

void ASmallEnemy::MoveToTarget(class AMain* Target)
{
	SetSmallEnemyMovementStatus(ESmallEnemyMovementStatus::EMS_MoveToTarget);

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

void ASmallEnemy::TakeDMG(float DamageValue, float KnockBackForce, FVector Direction)
{
	HP -= DamageValue; // Deals Damage
	LaunchCharacter(Direction * KnockBackForce, true, true); // launches enemy to make the player feel the force from the hit
	if (HP <= 0.f)
	{
		Destroy(); // Destroy the enemy if it looses all HP
	}
}

void ASmallEnemy::AttackBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	MainInHitRange = true;
	UE_LOG(LogTemp, Warning, TEXT("AttackBoxOnOverlapBegin()"));
}

void ASmallEnemy::AttackBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	MainInHitRange = false;
	UE_LOG(LogTemp, Warning, TEXT("AttackBoxOnOverlapEnd()"));
}

void ASmallEnemy::HitPlayer()
{
	if (MainInHitRange)
	{
		// cast TakeDMG function on player
		UE_LOG(LogTemp, Warning, TEXT("Player Got Hit!"));
	}
	UE_LOG(LogTemp, Warning, TEXT("HitPlayer()"));
}